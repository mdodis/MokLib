#include "Serialization/JSON.h"

#include "Reflection.h"
#include "Tape.h"
#include "Test/Test.h"

namespace CompoundStructMode {
    enum Type : u32
    {
        Mode0,
        Mode1,
        Mode2,
    };
}
typedef CompoundStructMode::Type ECompoundStructMode;

PROC_FMT_ENUM(CompoundStructMode, {
    FMT_ENUM_CASE(CompoundStructMode, Mode0);
    FMT_ENUM_CASE(CompoundStructMode, Mode1);
    FMT_ENUM_CASE(CompoundStructMode, Mode2);
    FMT_ENUM_DEFAULT_CASE_UNREACHABLE();
})

PROC_PARSE_ENUM(CompoundStructMode, {
    PARSE_ENUM_CASE(CompoundStructMode, Mode0);
    PARSE_ENUM_CASE(CompoundStructMode, Mode1);
    PARSE_ENUM_CASE(CompoundStructMode, Mode2);
})

struct FixedFloat3 {
    float e[3];

    float& operator[](int i) { return e[i]; }
};

struct CompoundStruct {
    FixedFloat3         fixed;
    ECompoundStructMode mode;
    Str                 name;
};

struct CompoundStructDescriptor : IDescriptor {
    FixedArrayDescriptor<FixedFloat3, float, 3> fixed_desc = {
        OFFSET_OF(CompoundStruct, fixed), LIT("fixed")};
    EnumDescriptor<ECompoundStructMode> mode_desc = {
        OFFSET_OF(CompoundStruct, mode), LIT("mode")};
    StrDescriptor name_desc = {OFFSET_OF(CompoundStruct, name), LIT("name")};

    IDescriptor* descs[3] = {
        &fixed_desc,
        &mode_desc,
        &name_desc,
    };

    CUSTOM_DESC_DEFAULT(CompoundStructDescriptor)
    virtual Str type_name() override { return LIT("CompoundStruct"); }
    virtual Slice<IDescriptor*> subdescriptors(umm self) override
    {
        return Slice<IDescriptor*>(descs, ARRAY_COUNT(descs));
    }
};

DEFINE_DESCRIPTOR_OF_INL(CompoundStruct)

TEST_CASE(
    "Lib/JSON", "{Compound struct with every possible type}, parses correctly")
{
    Str json = R"(
{
    "fixed": [0.0, 1.0, 2.0],
    "mode": "Mode0",
    "name": "hello"
}
)";

    RawTape        t(json);
    CompoundStruct c;
    REQUIRE(deserialize(&t, System_Allocator, c, json_deserialize), "");

    REQUIRE(c.fixed[0] == 0.0, "");
    REQUIRE(c.fixed[1] == 1.0, "");
    REQUIRE(c.fixed[2] == 2.0, "");
    REQUIRE(c.mode == CompoundStructMode::Mode0, "");
    REQUIRE(c.name == LIT("hello"), "");

    return MPASSED();
}