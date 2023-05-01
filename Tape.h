#pragma once
#include <stdio.h>
#include <string.h>

#include "Config.h"
#include "Math/Base.h"
#include "Memory/Base.h"
#include "Memory/RawBuffer.h"
#include "Str.h"
#include "Traits.h"

enum class ReadTapeMode : u32
{
    Read,
    Seek,
    End,
};

/**
 * i64 proc(void* dst, i64 size, Mode mode, void* usr)
 *
 * @param dst  The destination buffer
 * @param size The size of the destination buffer
 * @param Mode The mode:
 *        - Read: read to the destination buffer
 *        - Seek: seek by size
 *        - End: return 1 if at end
 * @return The number of bytes read, or the number of bytes moved
 */
#define PROC_READ_TAPE_READ(name) \
    i64 name(void* dst, i64 size, ReadTapeMode mode, void* usr)
typedef PROC_READ_TAPE_READ(ProcReadTapeRead);

struct MOKLIB_API ReadTape {
    ReadTape(ProcReadTapeRead* proc, void* usr) : proc(proc), usr(usr) {}

    ProcReadTapeRead* proc;
    void*             usr;

    _inline i64 read(void* dst, i64 size)
    {
        return proc(dst, size, ReadTapeMode::Read, usr);
    }

    _inline bool seek(i64 offset) { return try_seek(offset) == offset; }

    _inline i64 try_seek(i64 offset)
    {
        return proc(0, offset, ReadTapeMode::Seek, usr);
    }

    _inline bool end() { return proc(0, 0, ReadTapeMode::End, usr) == 1; }

    _inline char read_char()
    {
        char result;
        if (read(&result, sizeof(result)) != 1) {
            return EOF;
        }

        return result;
    }

    _inline bool peek_char(char match)
    {
        char result;
        if (read(&result, sizeof(result)) != 1) {
            return false;
        }

        if (result == match) {
            return true;
        } else {
            seek(-1);
            return false;
        }
    }

    _inline Str read_str(IAllocator& allocator, u64 len)
    {
        umm ptr = allocator.reserve(len);
        if (!ptr) return Str::NullStr;

        i64 num_read = read(ptr, (i64)len);
        if (num_read == 0) return Str::NullStr;

        return Str(ptr, (u64)num_read, false);
    }

    template <typename T>
    _inline bool read_struct(T& out)
    {
        return read(&out, sizeof(T)) == sizeof(T);
    }
};

enum class WriteTapeMode : u32
{
    Write,
    Seek,
    End
};

/**
 * i64 proc(void* src, i64 size, Mode mode, void* usr)
 *
 * @param src  The source buffer
 * @param size The size of the source buffer
 * @param Mode The mode:
 *        - Write: write from the source buffer
 *        - Seek: seek by size
 *        - End: return 1 if at end. Note that some tapes may never be at their
 * end
 * @return The number of bytes written, or the number of bytes moved
 */
#define PROC_WRITE_TAPE_WRITE(name) \
    i64 name(void* src, i64 size, WriteTapeMode mode, void* usr)
typedef PROC_WRITE_TAPE_WRITE(ProcWriteTapeWrite);

struct MOKLIB_API WriteTape {
    WriteTape(ProcWriteTapeWrite* proc, void* usr) : proc(proc), usr(usr) {}

    _inline i64 write(void* src, i64 size)
    {
        return proc(src, size, WriteTapeMode::Write, usr);
    }

    _inline bool write_char(char c) { return write(&c, 1) == 1; }

    _inline bool write_raw(const Raw& raw)
    {
        return write(raw.buffer, raw.size);
    }

    _inline bool write_str(const Str& str) { return write_raw((Raw)str); }

    _inline bool seek(i64 offset) { return try_seek(offset) == offset; }

    _inline i64 try_seek(i64 offset)
    {
        return proc(0, offset, WriteTapeMode::Seek, usr);
    }

    ProcWriteTapeWrite* proc;
    void*               usr;
};

struct MOKLIB_API RawReadTape : public ReadTape {
    RawReadTape(Raw buffer)
        : buffer(buffer)
        , offset(0)
        , ReadTape(RawReadTape::read_proc, (void*)this)
    {}

    static PROC_READ_TAPE_READ(read_proc)
    {
        RawReadTape* self = (RawReadTape*)usr;

        switch (mode) {
            case ReadTapeMode::Read: {
                if (size <= 0) return -1;

                u64 sizeu64 = (u64)size;

                if ((sizeu64 + self->offset) > self->buffer.size) {
                    sizeu64 = self->buffer.size - self->offset;
                }

                memcpy(dst, (u8*)self->buffer.buffer + self->offset, sizeu64);
                self->offset += sizeu64;

                return (i64)sizeu64;
            } break;

            case ReadTapeMode::Seek: {
                if (size < 0) {
                    u64 sizeu64 = (u64)(-size);

                    if (sizeu64 > self->offset) sizeu64 = self->offset;

                    self->offset -= sizeu64;

                    return -((i64)sizeu64);
                } else {
                    u64 sizeu64 = (u64)size;

                    if ((sizeu64 + self->offset) > self->buffer.size)
                        sizeu64 = self->buffer.size - self->offset;

                    self->offset += sizeu64;

                    return (i64)sizeu64;
                }
            } break;

            case ReadTapeMode::End: {
                return (self->offset == self->buffer.size) ? 1 : 0;
            } break;
        }
        return 0;
    }

    u64 offset;
    Raw buffer;
};

struct MOKLIB_API RawWriteTape : public WriteTape {
    RawWriteTape(Raw buffer)
        : buffer(buffer), offset(0), WriteTape(write_proc, (void*)this)
    {}

    static PROC_WRITE_TAPE_WRITE(write_proc)
    {
        RawWriteTape* self = (RawWriteTape*)usr;

        switch (mode) {
            case WriteTapeMode::Write: {
                if (size <= 0) return -1;
                u64 sizeu64 = (u64)size;

                if ((sizeu64 + self->offset) > self->buffer.size) {
                    sizeu64 = self->buffer.size - self->offset;
                }

                memcpy((u8*)self->buffer.buffer + self->offset, src, sizeu64);
                self->offset += sizeu64;

                return (i64)sizeu64;

            } break;

            case WriteTapeMode::Seek: {
                if (size < 0) {
                    u64 sizeu64 = (u64)(-size);

                    if (sizeu64 > self->offset) sizeu64 = self->offset;

                    self->offset -= sizeu64;

                    return -((i64)sizeu64);
                } else {
                    u64 sizeu64 = (u64)size;

                    if ((sizeu64 + self->offset) > self->buffer.size)
                        sizeu64 = self->buffer.size - self->offset;

                    self->offset += sizeu64;

                    return (i64)sizeu64;
                }
            } break;

            case WriteTapeMode::End: {
                return (self->offset == self->buffer.size) ? 1 : 0;
            } break;
        }
        return 0;
    }

    u64 offset;
    Raw buffer;
};

struct MOKLIB_API ParseReadTape : public ReadTape {
    ParseReadTape(ReadTape& tape)
        : tape(tape), num_read(0), ReadTape(read_proc, (void*)this)
    {}

    static PROC_READ_TAPE_READ(read_proc)
    {
        ParseReadTape* self = (ParseReadTape*)usr;
        switch (mode) {
            case ReadTapeMode::Read: {
                i64 off = self->tape.proc(dst, size, mode, self->tape.usr);
                self->num_read += off;
                return off;
            } break;

            case ReadTapeMode::Seek: {
                i64 off = self->tape.proc(0, size, mode, self->tape.usr);
                self->num_read += off;
                return off;
            } break;

            case ReadTapeMode::End: {
                return self->tape.proc(0, 0, mode, self->tape.usr);
            } break;
        }
        return 0;
    }

    void restore() { tape.seek(-num_read); }

    ReadTape& tape;
    i64       num_read;
};

struct MOKLIB_API MeasureWriteTape : public WriteTape {
    MeasureWriteTape() : WriteTape(write_proc, (void*)this) {}

    static PROC_WRITE_TAPE_WRITE(write_proc)
    {
        MeasureWriteTape* self = (MeasureWriteTape*)usr;

        switch (mode) {
            case WriteTapeMode::Write: {
                if (size <= 0) return 0;

                self->num_written += size;
                self->current_offset += size;

                return size;
            } break;

            case WriteTapeMode::Seek: {
                if (size < 0) {
                    u64 sizeu64 = (u64)(-size);
                    if (sizeu64 > self->current_offset) {
                        sizeu64 = self->current_offset;
                    }

                    self->current_offset -= sizeu64;
                    return -((i64)sizeu64);
                } else {
                    self->current_offset += size;
                    return size;
                }
            } break;

            case WriteTapeMode::End: {
                return 0;
            } break;

            default: {
                return 0;
            } break;
        }
    }

    /** The offset from the resulting writes + seeks */
    u64 current_offset = 0;
    /** The total number of bytes written */
    u64 num_written    = 0;
};

struct MOKLIB_API Tape {
    // Interface
    virtual u64  read(void* destination, u64 amount)   = 0;
    virtual bool write(const void* src, u64 num_bytes) = 0;
    virtual bool end()                                 = 0;
    virtual void move(i64 offset)                      = 0;

    char read_char();
    bool peek_char(char match);
    Str  read_line(IAllocator& alloc);

    // Helpers
    template <typename T>
    bool read_struct(T* destination)
    {
        return read(destination, sizeof(T)) == sizeof(T);
    }

    int64 read_raw(Raw& destination)
    {
        return read(destination.buffer, u32(destination.size));
    }

    bool write_char(char c) { return write(&c, 1); }

    bool write_str(const Str& str) { return write(str.data, str.len); }
};

struct SizedTape : public Tape {
    // Interface
    u64          current_offset, size;
    virtual bool end() override { return current_offset >= size; }

    virtual void move(i64 offset) override
    {
        if (offset < 0) {
            current_offset -= u64(-offset);
        } else {
            current_offset += offset;
        }
    }
};

struct RawTape : public SizedTape {
    Raw raw;

    RawTape(const Raw& raw) : raw(raw)
    {
        current_offset = 0;
        size           = raw.size;
    }

    RawTape(const Str& str) : RawTape(Raw((void*)str.data, str.len)) {}

    u64 read(void* destination, u64 amount) override
    {
        if (current_offset == size) {
            return NumProps<u64>::max;
        }

        u64 offset = current_offset;
        memcpy(destination, umm(raw.buffer) + offset, amount);
        current_offset += amount;
        return amount;
    }

    bool write(const void* src, u64 amount) override
    {
        if ((current_offset + amount) > size) {
            return false;
        }

        u64 offset = min(current_offset, size - current_offset);
        memcpy((umm)raw.buffer + current_offset, src, amount);
        current_offset += amount;
        return true;
    }
};

struct MeasureTape : public Tape {
    u64 num_written = 0;

    // Interface
    virtual u64  read(void* destination, u64 amount) override { return 0; }
    virtual bool write(const void* src, u64 num_bytes) override
    {
        num_written += num_bytes;
        return true;
    }
    virtual bool end() override { return false; }
    virtual void move(i64 offset) override {}
};

/**
 * Tape that keeps track of how many characters were read, and can reset the
 * included tape. Simplifies parsing.
 */
struct ParseTape : public Tape {
    i64   num_read = 0;
    Tape* tape;

    ParseTape(Tape* in_tape) : tape(in_tape) {}

    // Interface
    virtual u64 read(void* destination, u64 amount) override
    {
        u64 rd = tape->read(destination, amount);
        num_read -= rd;
        return rd;
    }

    virtual bool write(const void* src, u64 num_bytes) override
    {
        return false;
    }
    virtual bool end() override { return tape->end(); }
    virtual void move(i64 offset) override
    {
        tape->move(offset);
        num_read -= offset;
    }

    void restore() { tape->move(num_read); }
};
