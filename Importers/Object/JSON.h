#pragma once
#include "Base.h"
#include "Reflection.h"
#include "StringFormat.h"
#include "FileSystem/FileSystem.h"

/**
 * Serialize an object to JSON
 * @param  in    The tape object
 * @param  desc  Type Descriptor
 * @param  ptr   Pointer to type instance
 */
PROC_SERIALIZE(json_serialize_pretty);

/**
 * Deserialize an object from JSON
 * @param  output The output tape
 * @param  desc   Type Descriptor
 * @param  alloc Allocator for strings and any other object
 * @param  ptr    Pointer to type instance
 */
PROC_DESERIALIZE(json_deserialize);

/** Shorthand of json_parse */
template <typename T>
static _inline void json_input(Tape *from, IAllocator &alloc, T *result) {
    json_parse(from, alloc, descriptor_of(result), (umm)result);
}

/** Shorthand of json_serialize_pretty */
template <typename T>
static _inline void json_serialize_pretty(Tape *output, T *ptr) {
    IDescriptor *descriptor = descriptor_of(ptr);
    json_serialize_pretty(output, descriptor, (umm)ptr);
}

