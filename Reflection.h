#pragma once
#include "Base.h"
#include "Str.h"
#include "Containers/Slice.h"
#include "Containers/Array.h"
#include "Tape.h"

/**
 * Describes the abstract type class of a type. Used for (de)serialization
 */
enum class TypeClass {
    /** i32, f32, etc... */
    Primitive,
    /** Str */
    String,
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
struct IDescriptor {
    u64 offset;
    Str name;
    TypeClass type_class;

    constexpr IDescriptor(u64 offset, Str name, TypeClass type_class)
        : offset(offset)
        , name(name)
        , type_class(type_class)
        {}

    virtual Str type_name() = 0;
    virtual Slice<IDescriptor*> subdescriptors() = 0;

    IDescriptor *find_descriptor(Str name);
};

/**
 * Generic Array Descriptor interface
 * Can also be implemented with linked-lists.
 */
struct IArrayDescriptor : IDescriptor {
    constexpr IArrayDescriptor(u64 offset, Str name)
        : IDescriptor(offset, name, TypeClass::Array)
        {}

    virtual IDescriptor *get_subtype_descriptor() = 0;
    virtual void init(umm self, IAllocator &alloc) = 0;
    virtual umm add(umm self) = 0;
    virtual u64 size(umm self) = 0;
    virtual umm get(umm self, u64 index) = 0;
    virtual Str type_name() = 0;
    virtual Slice<IDescriptor*> subdescriptors() override { return {}; }
};

/**
 * Internal use: differenciate between compound data types and primitive types
 * like int, float...
 */
struct IPrimitiveDescriptor : IDescriptor {
    constexpr IPrimitiveDescriptor(u64 offset, Str name)
        : IDescriptor(offset, name, TypeClass::Primitive)
        {}

    virtual void format_primitive(Tape *out, umm ptr) = 0;
    virtual Slice<IDescriptor*> subdescriptors() override { return {}; }
};

/**
 * Describes a primitive value that has a fmt parameter
 */
template <typename T>
struct PrimitiveDescriptor : IPrimitiveDescriptor {
    constexpr PrimitiveDescriptor(u64 offset, Str name)
        : IPrimitiveDescriptor(offset, name)
        {}

    virtual void format_primitive(Tape *out, umm ptr) override {
        format(out, LIT("$"), *((T*)ptr));
    }

    virtual Str type_name() override { return MLITSTR(T); }
    virtual Slice<IDescriptor*> subdescriptors() override { return {}; }
};

/**
 * Descriptor for @Str
 */
struct StrDescriptor : IDescriptor {
    constexpr StrDescriptor(u64 offset, Str name)
        : IDescriptor(offset, name, TypeClass::String)
        {}

    virtual Str type_name() override { return LIT("Str"); }
    virtual Slice<IDescriptor*> subdescriptors() override { return {}; }
};

/**
 * Descriptor for TArray<T>
 */
template <typename T>
struct ArrayDescriptor : IArrayDescriptor {

    constexpr ArrayDescriptor(u64 offset, Str name)
        : IArrayDescriptor(offset, name)
        {}

    virtual IDescriptor *get_subtype_descriptor() override {
        return descriptor_of(((T*)0));
    }

    virtual void init(umm self, IAllocator &alloc) override {
        TArray<T> *ar = (TArray<T>*)self;
        *ar = TArray<T>(&alloc);
    }

    virtual umm add(umm self) override {
        TArray<T> *ar = (TArray<T>*)self;
        return (umm)ar->add();
    }

    virtual u64 size(umm self) override {
        TArray<T> *ar = (TArray<T>*)self;
        return ar->size;
    }

    virtual umm get(umm self, u64 index) override {
        TArray<T> &ar = as<TArray<T>>(self);
        return (umm)&ar[index];
    }

    virtual Str type_name() override {
        return LIT("Array <>");
    }
};

/**
 * Fetch the descriptor instance of the object type
 */
template <typename T>
constexpr static _inline IDescriptor *descriptor_of(T *what) {
    static_assert("No descriptor for this type." && false);
}

/**
 * Define the descriptor function and variable for the corresponding type.
 * Inline use only.
 * @param  T  The type descriptor
 */
#define DEFINE_DESCRIPTOR_OF(T)                                    \
    MCONCAT(T,Descriptor) MCONCAT2(T,_,Descriptor);                \
    template <>                                                    \
    constexpr static _inline IDescriptor *descriptor_of(T *what) { \
        return &MCONCAT2(T,_,Descriptor);                          \
    }

/**
 * Defines the default constructor of a custom descriptor object
 * @param  Type The type of the object
 */
#define CUSTOM_DESC_DEFAULT(Type)                           \
    constexpr Type(u64 offset = 0, Str name = Str::NullStr) \
        : IDescriptor(offset, name, TypeClass::Object)      \
        {}                                                  \

/**
 * Internal use: define descriptor for primitive types
 * @param  Type The primitive types
 */
#define DEFINE_PRIMITIVE_DESCRIPTOR(Type)                             \
    static PrimitiveDescriptor<Type> MCONCAT(Type,Descriptor) = {     \
        0, Str::NullStr                                               \
    };                                                                \
    template <>                                                       \
    static _inline constexpr IDescriptor *descriptor_of(Type *what) { \
        return &MCONCAT(Type,Descriptor);                             \
    }

/**
 * Defines the subdescriptors of the custom descriptor object. Use inside the
 * struct declaration/definition
 * @param  array The array to return
 */
#define REGISTER_SUBDESCRIPTORS(array)                         \
    virtual Slice<IDescriptor*> subdescriptors() override {    \
        return Slice<IDescriptor*>(array, ARRAY_COUNT(array)); \
    }

/**
 * Defines the type name of the custom descriptor object. Use inside the struct
 * declaration/definition
 * @param  string The literal name of the type (e.g. "MyObject")
 */
#define REGISTER_TYPENAME(string) \
    virtual Str type_name() override { return LIT(string); }

#define PROC_SERIALIZE(name) \
    void name(Tape *out, IDescriptor *desc, umm ptr)

#define PROC_DESERIALIZE(name) \
    void name(Tape *in, IAllocator &alloc, IDescriptor *desc, umm ptr)

typedef PROC_SERIALIZE(ProcSerialize);
typedef PROC_DESERIALIZE(ProcDeserialize);

DEFINE_PRIMITIVE_DESCRIPTOR(f32)
DEFINE_PRIMITIVE_DESCRIPTOR(i32)

/** Stores the object as well as the target path of the serialized instance */
template <typename T>
struct SerializedObject {
    T object;
    Str file_path;
    ProcSerialize *serialize;
    ProcDeserialize *deserialize;
    IDescriptor *descriptor;
    IAllocator *allocator;

    SerializedObject()
        : file_path(Str::NullStr)
        , serialize(0)
        , deserialize(0)
        , allocator(0)
        {}

    SerializedObject(
        Str file_path,
        ProcSerialize *serialize,
        ProcDeserialize *deserialize)
        : file_path(file_path)
        , serialize(serialize)
        , deserialize(deserialize)
        , allocator(0)
    {
        descriptor = descriptor_of(((T*)0));
    }

    void load() {
        FileHandle fh = open_file(
            file_path,
            FileMode::Read | FileMode::OpenAlways);

        if (!IS_VALID_FILE(fh)) {
            return;
        }

        DEFER(close_file(fh));

        FileTape ft(fh);
        deserialize(&ft, *allocator, descriptor, (umm)&object);
    }

    void flush() {
        FileHandle fh = open_file(
            file_path,
            FileMode::Write | FileMode::CreateAlways);
        DEFER(close_file(fh));

        FileTape ft(fh);
        serialize(&ft, descriptor, (umm)&object);
    }
};