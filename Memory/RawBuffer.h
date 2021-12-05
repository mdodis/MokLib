#pragma once
#include "../Base.h"

struct Raw {
    /** Pointer to buffer data */
    void *buffer;
    /** Size of the buffer (in bytes) */
    uint32 size;
};

struct RawTexture : public Raw {
    /** Distance (in bytes) from the beginning of one line to the next */
    uint32 pitch;
};

template <typename T>
struct TRaw {
    /** Pointer to buffer data */
    T *data;
    /** Size of the buffer (in elements) */
    u32 len;

    _inline constexpr operator Raw() const {
        return Raw {data, len * sizeof(T)};
    }

    _inline const T &operator[](u32 index) const {
        return data[index];
    }

    _inline T &operator[](u32 index) {
        return data[index];
    }
};

template <typename T>
static _inline TRaw<T> traw_from_array(T *data, u32 len) {
    return TRaw<T>{ data, len};
}

#define TRAW(static_array) traw_from_array(static_array, ARRAY_COUNT(static_array))