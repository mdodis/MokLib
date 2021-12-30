#pragma once

#define PUSH_STRUCT(alloc, type) (type*)alloc->reserve(sizeof(type))