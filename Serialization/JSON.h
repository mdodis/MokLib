#pragma once
#include "../Base.h"
#include "../Config.h"
#include "FileSystem/FileSystem.h"
#include "Serialization/Base.h"
#include "StringFormat.h"

/**
 * Serialize an object to JSON
 * @param  in    The tape object
 * @param  desc  Type Descriptor
 * @param  ptr   Pointer to type instance
 */
MOKLIB_API PROC_SERIALIZE(json_serialize_pretty);

/**
 * Deserialize an object from JSON
 * @param  output The output tape
 * @param  desc   Type Descriptor
 * @param  alloc  Allocator for strings and any other object
 * @param  ptr    Pointer to type instance
 */
MOKLIB_API PROC_DESERIALIZE(json_deserialize);

/** Shorthand of json_serialize_pretty */
template <typename T>
static _inline void json_serialize_pretty(WriteTape* output, T* ptr)
{
    IDescriptor* descriptor = descriptor_of(ptr);
    json_serialize_pretty(output, descriptor, (umm)ptr);
}
