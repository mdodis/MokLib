#pragma once

#define PUSH_STRUCT(alloc, type) (type*)alloc.reserve(alloc.context, sizeof(type))