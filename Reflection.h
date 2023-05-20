#pragma once
#include <typeinfo>

#include "Base.h"
#include "Config.h"
#include "Containers/Array.h"
#include "Containers/Slice.h"
#include "Str.h"
#include "StringFormat.h"
#include "Tape.h"
#include "Traits.h"

/**
 * Describes the abstract type class of a type. Used for (de)serialization
 */
enum class TypeClass
{
    /** i32, f32, etc... */
    Primitive,
    /** Str */
    String,
    /**
     * Slightly different than @String: Values are stored as strings, but
     * serialized into primitives
     */
    Enumeration,
    /** Any object with an array or vector-like behavior */
    Array,
    /** A compound object from any other set of type classes */
    Object
};

/**
 * Base class of any Descriptor object
 * Encapsulates everything that a generic object or struct descriptor should do
 * for serialization purposes.
 */
struct MOKLIB_API IDescriptor {
    u64       offset;
    Str       name;
    TypeClass type_class;

    constexpr IDescriptor(u64 offset, Str name, TypeClass type_class)
        : offset(offset), name(name), type_class(type_class)
    {}

    virtual Str                 type_name() const        = 0;
    virtual Slice<IDescriptor*> subdescriptors(umm self) = 0;

    IDescriptor* find_descriptor(umm self, Str name);
};

/**
 * Generic Array Descriptor interface
 * Can also be implemented with linked-lists.
 */
struct IArrayDescriptor : IDescriptor {
    constexpr IArrayDescriptor(u64 offset, Str name)
        : IDescriptor(offset, name, TypeClass::Array)
    {}

    virtual IDescriptor*        get_subtype_descriptor()         = 0;
    virtual void                init_read()                      = 0;
    virtual void                init(umm self, Allocator& alloc) = 0;
    virtual umm                 add(umm self)                    = 0;
    virtual u64                 size(umm self)                   = 0;
    virtual umm                 get(umm self, u64 index)         = 0;
    virtual Slice<IDescriptor*> subdescriptors(umm self) override { return {}; }
};

/**
 * Internal use: differentiate between compound data types and primitive types
 * like int, float...
 */
struct IPrimitiveDescriptor : IDescriptor {
    constexpr IPrimitiveDescriptor(u64 offset, Str name)
        : IDescriptor(offset, name, TypeClass::Primitive)
    {}

    virtual u32                 get_size()                                = 0;
    virtual void                format_primitive(WriteTape* out, umm ptr) = 0;
    virtual Slice<IDescriptor*> subdescriptors(umm self) override { return {}; }
};

struct IEnumDescriptor : IDescriptor {
    constexpr IEnumDescriptor(u64 offset, Str name, Str type_name)
        : IDescriptor(offset, name, TypeClass::Enumeration)
        , type_name_str(type_name)
    {}

    virtual void format_enum(WriteTape* out, umm ptr)                    = 0;
    virtual bool parse_enum(ReadTape* in, Allocator& allocator, umm ptr) = 0;

    virtual Str type_name() const override { return type_name_str; }
    virtual Slice<IDescriptor*> subdescriptors(umm self) override { return {}; }
    Str                         type_name_str;
};

/**
 * Fetch the descriptor instance of the object type
 */
template <typename T>
static _inline IDescriptor* descriptor_of(T* what);

template <typename T>
static _inline IDescriptor* descriptor_of_or_zero(T* what)
{
    if constexpr (HasDescriptor<T>::value) {
        return descriptor_of(what);
    } else {
        return nullptr;
    }
}

/**
 * Describes a primitive value that has a fmt parameter
 */
template <typename T>
struct PrimitiveDescriptor : IPrimitiveDescriptor {
    Str type_name_str;
    constexpr PrimitiveDescriptor(
        u64 offset, Str name, Str type_name = Str(typeid(T).name()))
        : IPrimitiveDescriptor(offset, name), type_name_str(type_name)
    {}

    virtual u32 get_size() override { return sizeof(T); }

    virtual void format_primitive(WriteTape* out, umm ptr) override
    {
        format(out, LIT("{}"), *((T*)ptr));
    }

    virtual Str type_name() const override { return type_name_str; }
    virtual Slice<IDescriptor*> subdescriptors(umm self) override { return {}; }
};

/**
 * Descriptor for @Str
 */
struct StrDescriptor : IDescriptor {
    constexpr StrDescriptor(u64 offset, Str name)
        : IDescriptor(offset, name, TypeClass::String)
    {}

    virtual Str type_name() const override { return LIT("Str"); }
    virtual Slice<IDescriptor*> subdescriptors(umm self) override { return {}; }
};

template <typename T>
struct EnumDescriptor : IEnumDescriptor {
    constexpr EnumDescriptor(
        u64 offset, Str name, Str type_name = Str(typeid(T).name()))
        : IEnumDescriptor(offset, name, type_name)
    {}

    virtual void format_enum(WriteTape* out, umm ptr) override
    {
        format(out, LIT("{}"), *((T*)ptr));
    }

    virtual bool parse_enum(
        ReadTape* in, Allocator& allocator, umm ptr) override
    {
        return parse<T>(in, *((T*)ptr), allocator);
    }
};

/**
 * A fixed array descriptor that can be used on structs with only one
 * type of member; as if it was an array (for example a Vector of 3 floats)
 *
 * @param StorageType    The actual storage type of the whole struct
 * @param StorageSubType The type of each element
 * @param Count          The max count of elements
 *
 * @remark StorageType needs to have implemented the following:
 * - Array index operator
 * - Default constructor
 */
template <typename StorageType, typename StorageSubType, u64 Count>
struct FixedArrayDescriptor : IArrayDescriptor {
    constexpr FixedArrayDescriptor(u64 offset, Str name)
        : IArrayDescriptor(offset, name)
        , type_name_str(Str(typeid(StorageType).name()))
        , i(Count)
    {}

    virtual IDescriptor* get_subtype_descriptor() override
    {
        return descriptor_of(((StorageSubType*)0));
    }

    virtual void init_read() override { i = Count; }

    virtual void init(umm self, Allocator& alloc) override
    {
        i               = 0;
        StorageType* ar = (StorageType*)self;
        *ar             = StorageType();
    }

    virtual umm add(umm self) override
    {
        if (i >= Count) return 0;

        StorageType&    ar  = *((StorageType*)self);
        StorageSubType* sub = (StorageSubType*)(&ar[i]);

        i++;
        return (umm)sub;
    }

    virtual u64 size(umm self) override { return i; }

    virtual umm get(umm self, u64 index) override
    {
        if (index >= Count) return 0;

        StorageType& ar = *(StorageType*)self;
        return (umm)(&ar[(int)index]);
    }

    virtual Str type_name() const override { return type_name_str; }

    int i;
    Str type_name_str;
};

/**
 * Descriptor for a simple c-style array
 */
template <typename StorageSubType, u64 Count>
struct StaticArrayDescriptor : IArrayDescriptor {
    using StorageType = StorageSubType[Count];

    constexpr StaticArrayDescriptor(u64 offset, Str name)
        : IArrayDescriptor(offset, name)
        , type_name_str(Str(typeid(StorageType).name()))
        , i(Count)
    {}

    virtual IDescriptor* get_subtype_descriptor() override
    {
        return descriptor_of(((StorageType*)0));
    }

    virtual void init_read() override { i = Count; }

    virtual void init(umm self, Allocator& alloc) override
    {
        i               = 0;
        StorageType* ar = (StorageType*)self;
        *ar             = StorageType();
    }

    virtual umm add(umm self) override
    {
        if (i >= Count) return 0;

        StorageType&    ar  = *((StorageType*)self);
        StorageSubType* sub = &ar[i];

        i++;
        return (umm)sub;
    }

    virtual u64 size(umm self) override { return i; }

    virtual umm get(umm self, u64 index) override
    {
        if (index >= Count) return 0;

        StorageType& ar = *(StorageType*)self;
        return (umm)(&ar[(int)index]);
    }

    virtual Str type_name() const override { return type_name_str; }

    int i;
    Str type_name_str;
};

/**
 * Descriptor for TArray<T>
 */
template <typename T>
struct ArrayDescriptor : IArrayDescriptor {
    constexpr ArrayDescriptor(u64 offset, Str name)
        : IArrayDescriptor(offset, name)
    {}

    virtual IDescriptor* get_subtype_descriptor() override
    {
        return descriptor_of(((T*)0));
    }

    virtual void init_read() override {}

    virtual void init(umm self, Allocator& alloc) override
    {
        TArray<T>* ar = (TArray<T>*)self;
        *ar           = TArray<T>(&alloc);
    }

    virtual umm add(umm self) override
    {
        TArray<T>* ar = (TArray<T>*)self;
        return (umm)ar->add();
    }

    virtual u64 size(umm self) override
    {
        TArray<T>* ar = (TArray<T>*)self;
        return ar->size;
    }

    virtual umm get(umm self, u64 index) override
    {
        TArray<T>& ar = as<TArray<T>>(self);
        return (umm)&ar[index];
    }

    virtual Str type_name() const override { return LIT("Array <>"); }
};

/**
 * Define the descriptor function and variable for the corresponding type.
 * Inline use only.
 * @param  T  The type descriptor
 */
#define DEFINE_DESCRIPTOR_OF(T) \
    MCONCAT(T, Descriptor) MCONCAT2(T, _, Descriptor);

/**
 * Declare the descriptor object of the type
 * @param  T  The type descriptor
 */
#define DECLARE_DESCRIPTOR_OF(T)                                       \
    ENABLE_DESCRIPTOR(T);                                              \
    extern MCONCAT(T, Descriptor) MCONCAT2(T, _, Descriptor);          \
    template <>                                                        \
    IDescriptor* descriptor_of(T* what)                                \
    {                                                                  \
        return static_cast<IDescriptor*>(&MCONCAT2(T, _, Descriptor)); \
    }

#define DEFINE_DESCRIPTOR_OF_INL(T)                           \
    ENABLE_DESCRIPTOR(T);                                     \
    static MCONCAT(T, Descriptor) MCONCAT2(T, _, Descriptor); \
    template <>                                               \
    IDescriptor* descriptor_of(T* what)                       \
    {                                                         \
        return &MCONCAT2(T, _, Descriptor);                   \
    }

/**
 * Defines the default constructor of a custom descriptor object
 * @param  Type The type of the object
 */
#define CUSTOM_DESC_DEFAULT(Type)                      \
    Type(u64 offset = 0, Str name = Str::NullStr)      \
        : IDescriptor(offset, name, TypeClass::Object) \
    {}

#define CUSTOM_DESC_OBJECT_DEFAULT_(Type) MCONCAT(Type, Descriptor)
#define CUSTOM_DESC_OBJECT_DEFAULT(Type, descs)                   \
    CUSTOM_DESC_OBJECT_DEFAULT_(Type)                             \
    (u64 offset = 0, Str name = Str::NullStr)                     \
        : IDescriptor(offset, name, TypeClass::Object)            \
    {}                                                            \
    virtual Str type_name() const override                        \
    {                                                             \
        return LIT(MSTR(Type));                                   \
    }                                                             \
    virtual Slice<IDescriptor*> subdescriptors(umm self) override \
    {                                                             \
        return Slice<IDescriptor*>(descs, ARRAY_COUNT(descs));    \
    }

/**
 * Internal use: define descriptor for primitive types
 * @param  Type The primitive types
 */
#define DEFINE_PRIMITIVE_DESCRIPTOR(Type)                        \
    static PrimitiveDescriptor<Type> MCONCAT(Type, Descriptor) = \
        {0, Str::NullStr, #Type};                                \
    template <>                                                  \
    IDescriptor* descriptor_of(Type* what)                       \
    {                                                            \
        return &MCONCAT(Type, Descriptor);                       \
    }

#define DEFINE_ARRAY_DESCRIPTOR_INL(VType)                              \
    static ArrayDescriptor<VType> MCONCAT2(Array_, VType, Descriptor) = \
        {0, Str::NullStr};                                              \
    template <>                                                         \
    IDescriptor* descriptor_of(TArray<VType>* what)                     \
    {                                                                   \
        return &MCONCAT2(Array_, VType, Descriptor);                    \
    }

/**
 * Defines the subdescriptors of the custom descriptor object. Use inside the
 * struct declaration/definition
 * @param  array The array to return
 */
#define REGISTER_SUBDESCRIPTORS(array)                         \
    virtual Slice<IDescriptor*> subdescriptors() override      \
    {                                                          \
        return Slice<IDescriptor*>(array, ARRAY_COUNT(array)); \
    }

/**
 * Defines the type name of the custom descriptor object. Use inside the struct
 * declaration/definition
 * @param  string The literal name of the type (e.g. "MyObject")
 */
#define REGISTER_TYPENAME(string)    \
    virtual Str type_name() override \
    {                                \
        return LIT(string);          \
    }

DEFINE_PRIMITIVE_DESCRIPTOR(f32)
DEFINE_PRIMITIVE_DESCRIPTOR(i64)
DEFINE_PRIMITIVE_DESCRIPTOR(u64)
DEFINE_PRIMITIVE_DESCRIPTOR(i32)
DEFINE_PRIMITIVE_DESCRIPTOR(u32)
DEFINE_PRIMITIVE_DESCRIPTOR(i16)
DEFINE_PRIMITIVE_DESCRIPTOR(u16)
DEFINE_PRIMITIVE_DESCRIPTOR(i8)
DEFINE_PRIMITIVE_DESCRIPTOR(u8)
DEFINE_PRIMITIVE_DESCRIPTOR(bool)

static StrDescriptor Str_Descriptor = {0, Str::NullStr};
template <>
IDescriptor* descriptor_of(Str* what)
{
    return &Str_Descriptor;
}