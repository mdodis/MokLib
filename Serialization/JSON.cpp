#include "JSON.h"
#include "Containers/Array.h"
#include "Variant.h"
#include "StringFormat.h"
#include "Parsing.h"

#if COMPILER_MSVC
    #pragma warning(push)
    #pragma warning(disable: 4533)
#endif

static bool parse_object(Tape *in, IAllocator &alloc, DescPair pair);
static bool parse_qstr(Tape *in, IAllocator &alloc, Str &result);
static bool parse_value(Tape *in, IAllocator &alloc, DescPair pair);
static bool parse_number(Tape *in, IAllocator &alloc, DescPair pair);
static bool parse_string(Tape *in, IAllocator &alloc, DescPair pair);
static bool parse_array(Tape *in, IAllocator &alloc, DescPair pair);

PROC_DESERIALIZE(json_deserialize) {

    // @todo: Check first if we're parsing an object or an array instead of
    // doing this.
    if (parse_object(in, alloc, DescPair{ desc, ptr })) {
        return;
    }
    parse_array(in, alloc, DescPair{ desc, ptr });
}

static bool parse_object(Tape *in, IAllocator &alloc, DescPair pair) {
    ParseTape tape(in);
    bool should_exit = false;

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
        if (!subpair)
            goto P_FAIL;

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

static bool parse_qstr(Tape *in, IAllocator &alloc, Str &result) {

    StrFormatter format_result(result, &alloc);
    return parse<StrFormatter>(in, format_result);
}

static bool parse_value(Tape *in, IAllocator &alloc, DescPair pair) {
    eat_whitespace(in);

    char c = in->read_char();

    switch (c) {

        case '{':  {
            in->move(-1);
            return parse_object(in, alloc, pair);
        } break;

        case '\"': {
            in->move(-1);
            return parse_string(in, alloc, pair);
        } break;

        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9': {
            in->move(-1);
            return parse_number(in, alloc, pair);
        } break;

        case '[': {
            in->move(-1);
            return parse_array(in, alloc, pair);
        } break;

        default: {} break;
    }

    return false;
}

static bool parse_number(Tape *in, IAllocator &alloc, DescPair pair) {
    if (IS_A(pair.desc, PrimitiveDescriptor<f32>)) {
        return parse<f32>(in, (*(float*)pair.ptr));
    }

    return false;
}

static bool parse_string(Tape *in, IAllocator &alloc, DescPair pair) {

    if (!IS_A(pair.desc, StrDescriptor)) {
        return false;
    }

    Str result;
    bool success = parse_qstr(in, alloc, result);
    if (!success) return false;

    *(Str*)pair.ptr = result;

    return true;
}

static bool parse_array(Tape *in, IAllocator &alloc, DescPair pair) {

    if (!IS_A(pair.desc, IArrayDescriptor)) {
        return false;
    }

    eat_whitespace(in);

    if (!expect(in, '[')) {
        return false;
    }

    bool should_exit = false;
    auto &desc = as<IArrayDescriptor>(pair.desc);

    // If this is an empty array, quit out
    eat_whitespace(in);
    if (in->peek_char(']')) {
        goto PARSE_EXIT;
    }

    desc.init(pair.ptr, alloc);

    while (!should_exit) {

        eat_whitespace(in);

        IDescriptor *subtype_desc = desc.get_subtype_descriptor();

        char c = in->read_char();
        if (c == EOF) {
            break;
        }

        in->move(-1);

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

static void json_serialize_pretty_space(Tape *out, IDescriptor *desc, umm ptr, u32 space);

static void json_output_pretty_value(Tape *output, IDescriptor *desc, umm ptr, u32 space) {

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
            auto &ar = as<IArrayDescriptor>(desc);
            IDescriptor *subtype_desc = ar.get_subtype_descriptor();

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
            StrFormatter sf(as<Str>(ptr), 0);
            format(output, LIT("$"), sf);
        } break;

        case TypeClass::Object: {
            json_serialize_pretty_space(output, desc, ptr, space);
        } break;

    }
}

PROC_SERIALIZE(json_serialize_pretty) {
    json_serialize_pretty_space(out, desc, ptr, 0);
}

static void json_serialize_pretty_space(Tape *out, IDescriptor *desc, umm ptr, u32 space) {
    Slice<IDescriptor*> descs = desc->subdescriptors();

    auto print_spaces = [&](u32 s) {
        for (u32 i = 0; i < s; ++i) {
            format(out, LIT(" "));
        }
    };

    format(out, LIT("{\n"));
    for (IDescriptor *sub : descs) {

        umm p = ptr + sub->offset;

        print_spaces(space + 4);
        format(out, LIT("\"$\": "), sub->name);

        json_output_pretty_value(out, sub, p, space + 4);

        if (sub != *descs.last())
            format(out, LIT(","));
        format(out, LIT("\n"));
    }
    print_spaces(space);
    format(out, LIT("}"));
}



#if COMPILER_MSVC
    #pragma warning(pop)
#endif
