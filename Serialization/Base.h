#pragma once
#include "FileSystem/FileSystem.h"
#include "Reflection.h"

#define PROC_SERIALIZE(name) \
    bool name(WriteTape* out, IDescriptor* desc, umm ptr)

#define PROC_DESERIALIZE(name) \
    bool name(ReadTape* in, Allocator& alloc, IDescriptor* desc, umm ptr)

typedef PROC_SERIALIZE(ProcSerialize);
typedef PROC_DESERIALIZE(ProcDeserialize);

template <typename T>
bool serialize(WriteTape* in, T& object, ProcSerialize* proc)
{
    return proc(in, descriptor_of<T>(&object), (umm)&object);
}

template <typename T>
bool deserialize(
    ReadTape* in, Allocator& alloc, T& object, ProcDeserialize* proc)
{
    return proc(in, alloc, descriptor_of<T>(&object), (umm)&object);
}

struct DescPair {
    IDescriptor* desc;
    umm          ptr;

    _inline operator bool() { return ptr != 0; }
};

static _inline DescPair get_descriptor(DescPair& parent, const Str& key)
{
    IDescriptor* subdescriptor = parent.desc->find_descriptor(parent.ptr, key);
    if (!subdescriptor) {
        return DescPair{0, 0};
    }

    return DescPair{subdescriptor, parent.ptr + subdescriptor->offset};
}

/** Stores the object as well as the target path of the serialized instance */
template <typename T>
struct SerializedObject {
    T                object;
    Str              file_path;
    ProcSerialize*   serialize;
    ProcDeserialize* deserialize;
    IDescriptor*     descriptor;
    Allocator*       allocator;

    SerializedObject()
        : file_path(Str::NullStr), serialize(0), deserialize(0), allocator(0)
    {}

    SerializedObject(
        Str file_path, ProcSerialize* serialize, ProcDeserialize* deserialize)
        : file_path(file_path)
        , serialize(serialize)
        , deserialize(deserialize)
        , allocator(0)
    {
        descriptor = descriptor_of(((T*)0));
    }

    T* operator->() { return &object; }

    void load()
    {
        FileHandle fh = open_file(file_path, FileMode::Read);

        if (!IS_VALID_FILE(fh)) {
            return;
        }

        DEFER(close_file(fh));

        FileReadTape ft(fh);
        ASSERT(deserialize(&ft, *allocator, descriptor, (umm)&object));
    }

    void flush()
    {
        FileHandle fh =
            open_file(file_path, FileMode::Write | FileMode::Truncate);
        DEFER(close_file(fh));

        FileWriteTape ft(fh);
        serialize(&ft, descriptor, (umm)&object);
    }
};