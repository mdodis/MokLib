#include "JSON.h"

#include "../Containers/Array.h"
#include "../Parsing.h"
#include "../StringFormat.h"
#include "../Variant.h"

#if COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable : 4533)
#endif

static bool parse_object(ReadTape* in, Allocator& alloc, DescPair pair);
static bool parse_qstr(ReadTape* in, Allocator& alloc, Str& result);
static bool parse_value(ReadTape* in, Allocator& alloc, DescPair pair);
static bool parse_number(ReadTape* in, Allocator& alloc, DescPair pair);
static bool parse_bool(ReadTape* in, Allocator& alloc, DescPair pair);
static bool parse_string(ReadTape* in, Allocator& alloc, DescPair pair);
static bool parse_array(ReadTape* in, Allocator& alloc, DescPair pair);

PROC_DESERIALIZE(json_deserialize)
{
    // @todo: Check first if we're parsing an object or an array instead of
    // doing this.
    //
    // Technically, it should always be an object first, with an array inside of
    // it, but this is not the case sometimes...
    if (parse_object(in, alloc, DescPair{desc, ptr})) {
        return true;
    }
    return parse_array(in, alloc, DescPair{desc, ptr});
}

static bool parse_object(ReadTape* in, Allocator& alloc, DescPair pair)
{
    ParseReadTape tape(*in);
    bool          should_exit = false;

    eat_whitespace(&tape);

    if (!expect(&tape, '{')) {
        goto P_FAIL;
    }

    while (!should_exit) {
        eat_whitespace(&tape);

        // Parse Key
        Str key;
        if (!parse_qstr(&tape, alloc, key)) {
            // We must parse an object
            goto P_FAIL;
        }

        DescPair subpair = get_descriptor(pair, key);
        if (!subpair) goto P_FAIL;

        eat_whitespace(&tape);

        if (!expect(&tape, ':')) {
            goto P_FAIL;
        }

        // Parse value
        if (!parse_value(&tape, alloc, subpair)) {
            goto P_FAIL;
        }

        eat_whitespace(&tape);

        if (!expect(&tape, ',')) {
            should_exit = true;
        }
    }

    if (!expect(&tape, '}')) {
        goto P_FAIL;
    }

    return true;

P_FAIL:
    tape.restore();
    return false;
}

static bool parse_qstr(ReadTape* in, Allocator& alloc, Str& result)
{
    StrFormatter format_result(result, &alloc);
    return parse<StrFormatter>(in, format_result);
}

static bool parse_value(ReadTape* in, Allocator& alloc, DescPair pair)
{
    eat_whitespace(in);

    char c = in->read_char();

    switch (c) {
        case '{': {
            in->seek(-1);
            return parse_object(in, alloc, pair);
        } break;

        case '\"': {
            in->seek(-1);
            return parse_string(in, alloc, pair);
        } break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            in->seek(-1);
            return parse_number(in, alloc, pair);
        } break;

        case 't':
        case 'f': {
            in->seek(-1);
            return parse_bool(in, alloc, pair);
        } break;

        case '[': {
            in->seek(-1);
            return parse_array(in, alloc, pair);
        } break;

        default: {
        } break;
    }

    return false;
}

template <typename NumType>
bool try_parse_primitive_type(ReadTape* in, DescPair pair)
{
    if constexpr (HasFmt<NumType>::value) {
        if (dynamic_cast<PrimitiveDescriptor<NumType>*>(pair.desc)) {
            return parse<NumType>(in, (*(NumType*)pair.ptr));
        }
    }
    return false;
}

static bool parse_number(ReadTape* in, Allocator& alloc, DescPair pair)
{
    if (try_parse_primitive_type<i64>(in, pair)) return true;
    if (try_parse_primitive_type<u64>(in, pair)) return true;
    if (try_parse_primitive_type<i32>(in, pair)) return true;
    if (try_parse_primitive_type<u32>(in, pair)) return true;
    if (try_parse_primitive_type<i16>(in, pair)) return true;
    if (try_parse_primitive_type<u16>(in, pair)) return true;
    if (try_parse_primitive_type<i8>(in, pair)) return true;
    if (try_parse_primitive_type<u8>(in, pair)) return true;
    if (try_parse_primitive_type<f32>(in, pair)) return true;

    return false;
}

static bool parse_bool(ReadTape* in, Allocator& alloc, DescPair pair)
{
    if (IS_A(pair.desc, PrimitiveDescriptor<bool>)) {
        return parse<bool>(in, (*(bool*)pair.ptr));
    }
    return false;
}

static bool parse_string(ReadTape* in, Allocator& alloc, DescPair pair)
{
    if (IS_A(pair.desc, StrDescriptor)) {
        Str  result;
        bool success = parse_qstr(in, alloc, result);
        if (!success) return false;

        *(Str*)pair.ptr = result;
    } else if (IS_A(pair.desc, IEnumDescriptor)) {
        if (!expect(in, '"')) {
            return false;
        }

        bool success =
            as<IEnumDescriptor>(pair.desc).parse_enum(in, alloc, pair.ptr);

        if (!expect(in, '"')) {
            return false;
        }

        return success;
    } else {
        return false;
    }

    return true;
}

static bool parse_array(ReadTape* in, Allocator& alloc, DescPair pair)
{
    if (!IS_A(pair.desc, IArrayDescriptor)) {
        return false;
    }

    eat_whitespace(in);

    if (!expect(in, '[')) {
        return false;
    }

    bool  should_exit = false;
    auto& desc        = as<IArrayDescriptor>(pair.desc);

    // If this is an empty array, quit out
    eat_whitespace(in);
    if (in->peek_char(']')) {
        goto PARSE_EXIT;
    }

    desc.init(pair.ptr, alloc);

    while (!should_exit) {
        eat_whitespace(in);

        IDescriptor* subtype_desc = desc.get_subtype_descriptor();

        char c = in->read_char();
        if (c == EOF) {
            break;
        }

        in->seek(-1);

        umm sub_ptr = desc.add(pair.ptr);

        if (!parse_value(in, alloc, DescPair{subtype_desc, sub_ptr})) {
            return false;
        }

        if (!expect(in, ',')) {
            should_exit = true;
        }
    }

    eat_whitespace(in);
    if (!expect(in, ']')) {
        return false;
    }

PARSE_EXIT:
    return true;
}

static bool json_serialize_pretty_space(
    WriteTape* out, IDescriptor* desc, umm ptr, u32 space);

static bool json_output_pretty_value(
    WriteTape* output, IDescriptor* desc, umm ptr, u32 space)
{
    auto print_spaces = [&](u32 s) {
        for (u32 i = 0; i < s; ++i) {
            format(output, LIT(" "));
        }
    };

    switch (desc->type_class) {
        case TypeClass::Primitive: {
            as<IPrimitiveDescriptor>(desc).format_primitive(output, ptr);
        } break;

        case TypeClass::Array: {
            auto&        ar           = as<IArrayDescriptor>(desc);
            IDescriptor* subtype_desc = ar.get_subtype_descriptor();
            ar.init_read();

            format(output, LIT("[\n"));
            for (u64 i = 0; i < ar.size(ptr); ++i) {
                umm elem_ptr = ar.get(ptr, i);

                print_spaces(space + 4);
                json_output_pretty_value(
                    output,
                    subtype_desc,
                    elem_ptr,
                    space + 4);

                if (i != (ar.size(ptr) - 1)) {
                    format(output, LIT(","));
                }
                format(output, LIT("\n"));
            }
            print_spaces(space);
            format(output, LIT("]\n"));
        } break;

        case TypeClass::String: {
            TFmtStr<FmtPolicy::WithQuotes> sf(as<Str>(ptr));
            format(output, LIT("{}"), sf);
        } break;

        case TypeClass::Enumeration: {
            output->write_str(LIT("\""));
            as<IEnumDescriptor>(desc).format_enum(output, ptr);
            output->write_str(LIT("\""));
        } break;

        case TypeClass::Object: {
            json_serialize_pretty_space(output, desc, ptr, space);
        } break;
    }

    return true;
}

PROC_SERIALIZE(json_serialize_pretty)
{
    return json_serialize_pretty_space(out, desc, ptr, 0);
}

static bool json_serialize_pretty_space(
    WriteTape* out, IDescriptor* desc, umm ptr, u32 space)
{
    Slice<IDescriptor*> descs = desc->subdescriptors(ptr);

    auto print_spaces = [&](u32 s) {
        for (u32 i = 0; i < s; ++i) {
            format(out, LIT(" "));
        }
    };

    format(out, LIT("{\n"));
    for (IDescriptor* sub : descs) {
        umm p = ptr + sub->offset;

        print_spaces(space + 4);
        format(out, LIT("\"{}\": "), sub->name);

        json_output_pretty_value(out, sub, p, space + 4);

        if (sub != *descs.last()) format(out, LIT(","));
        format(out, LIT("\n"));
    }
    print_spaces(space);
    format(out, LIT("}"));

    return true;
}

#if COMPILER_MSVC
#pragma warning(pop)
#endif
