#define MOK_LIB_STATIC 1
#include "Base.h"
#include "FileSystem/FileSystem.h"
#include "Importers/Object/JSON.h"

struct StrStruct {
    Str s;
};

struct FloatStruct {
    f32         a, b;
    StrStruct   s;
    TArray<f32> ar;
};

struct StrStructDescriptor : IDescriptor {
    CUSTOM_DESC_DEFAULT(StrStructDescriptor)

    StrDescriptor desc_s = {OFFSET_OF(StrStruct, s), LIT("s")};

    IDescriptor* descs[1] = {
        &desc_s,
    };

    virtual Str type_name() override { return LIT("StrStruct"); }
    virtual Slice<IDescriptor*> subdescriptors() override
    {
        return Slice<IDescriptor*>(descs, ARRAY_COUNT(descs));
    }
};

struct FloatStructDescriptor : IDescriptor {
    PrimitiveDescriptor<f32> desc_a  = {OFFSET_OF(FloatStruct, a), LIT("a")};
    PrimitiveDescriptor<f32> desc_b  = {OFFSET_OF(FloatStruct, b), LIT("b")};
    StrStructDescriptor      desc_s  = {OFFSET_OF(FloatStruct, s), LIT("s")};
    ArrayDescriptor<f32>     desc_ar = {OFFSET_OF(FloatStruct, ar), LIT("ar")};

    IDescriptor* descs[4] = {
        &desc_a,
        &desc_b,
        &desc_s,
        &desc_ar,
    };

    CUSTOM_DESC_DEFAULT(FloatStructDescriptor)
    REGISTER_SUBDESCRIPTORS(descs)
    REGISTER_TYPENAME("FloatStruct")
};

FloatStructDescriptor Float_Struct_Descriptor;

template <>
constexpr _inline IDescriptor* descriptor_of(FloatStruct* what)
{
    return &Float_Struct_Descriptor;
}

static void float_struct(Str file);
static void advanced_struct(Str file);

int main(int argc, char* argv[])
{
    float_struct(Str(argv[1]));

    advanced_struct(Str(argv[2]));
}

static void float_struct(Str file)
{
    auto fh = open_file(file, FileMode::Read | FileMode::OpenAlways);
    DEFER(close_file(fh));
    FileTape ft(fh);

    FloatStruct fs;
    StreamTape  out = get_stream(Console::Output);

    json_input(&ft, *System_Allocator, &fs);
    json_output_pretty(&out, descriptor_of(&fs), (umm)&fs);
}

static void advanced_struct(Str file)
{
    auto fh = open_file(file, FileMode::Read | FileMode::OpenAlways);
    DEFER(close_file(fh));
    FileTape ft(fh);

    StreamTape out = get_stream(Console::Output);
}

#include "Compile.inc"
#include "Importers/Object/JSON.cpp"
