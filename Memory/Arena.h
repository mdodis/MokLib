#pragma once
#include <string.h>

#include <utility>

#include "../Defer.h"
#include "Base.h"

namespace ArenaMode {
    enum Type
    {
        Fixed,
        Dynamic
    };
}
typedef ArenaMode::Type EArenaMode;

template <EArenaMode Mode = ArenaMode::Dynamic>
struct MOKLIB_API Arena;

template <EArenaMode Mode>
struct MOKLIB_API ArenaSave;

template <>
struct MOKLIB_API Arena<ArenaMode::Fixed> : public Allocator {
    Arena(Allocator& base, u64 capacity)
        : Allocator(allocate_proc, (void*)this), capacity(capacity), base(&base)
    {}
    Arena() : Arena(System_Allocator, KILOBYTES(1)) {}

    Arena(const Arena& copy)
        : Allocator(allocate_proc, (void*)this)
        , capacity(copy.capacity)
        , base(copy.base)
        , used(copy.used)
        , last_offset(copy.last_offset)
    {}

    Arena& operator=(const Arena& copy)
    {
        capacity    = copy.capacity;
        base        = copy.base;
        last_offset = copy.last_offset;
        used        = copy.used;
        return *this;
    }

    static PROC_ALLOCATOR_ALLOCATE(allocate_proc)
    {
        Arena* self = (Arena*)usr;

        if (self->memory == nullptr) {
            self->init();
        }

        switch (mode) {
            case AllocatorMode::Reserve: {
                return self->push(new_size);
            } break;

            case AllocatorMode::Resize: {
                return self->copy_or_resize(ptr, prev_size, new_size);
            } break;

            case AllocatorMode::Release: {
                self->try_pop(ptr);
                return 0;
            } break;
        }
        return 0;
    }

    void  init() { memory = base->reserve(capacity); }
    void* push(u64 size)
    {
        if ((used + size) > capacity) {
            return nullptr;
        }

        void* result = (u8*)memory + used;
        last_offset  = used;
        used += size;
        return result;
    }

    void* copy_or_resize(void* ptr, u64 prev, u64 size)
    {
        if (ptr == nullptr) return push(size);

        u8* last_ptr = (u8*)memory + last_offset;

        if ((last_ptr == ptr) && ((last_offset + size) <= capacity)) {
            used = last_offset + size;
            return last_ptr;
        } else {
            void* new_ptr = push(size);
            if (new_ptr == nullptr) return nullptr;

            memcpy(new_ptr, ptr, prev);
            return new_ptr;
        }
    }

    void try_pop(void* ptr)
    {
        u8* last_ptr = (u8*)memory + last_offset;

        if (last_ptr == ptr) {
            used        = last_offset;
            last_offset = 0;
        }
    }

    void deinit()
    {
        base->release(memory);
        memory      = nullptr;
        used        = 0;
        last_offset = 0;
    }

    void reset()
    {
        used        = 0;
        last_offset = 0;
    }

    void*      memory      = nullptr;
    u64        used        = 0;
    u64        last_offset = 0;
    u64        capacity;
    Allocator* base;
};

template <>
struct MOKLIB_API ArenaSave<ArenaMode::Fixed> {
    u64                      used;
    u64                      last_offset;
    Arena<ArenaMode::Fixed>* a;

    ArenaSave(Arena<ArenaMode::Fixed>& arena)
    {
        a           = &arena;
        used        = arena.used;
        last_offset = arena.last_offset;
    }

    ~ArenaSave()
    {
        a->used        = used;
        a->last_offset = last_offset;
    }
};

template <>
struct MOKLIB_API Arena<ArenaMode::Dynamic> : public Allocator {
    Arena(Allocator& base, u64 capacity)
        : Allocator(allocate_proc, (void*)this), capacity(capacity), base(&base)
    {}
    Arena() : Arena(System_Allocator, KILOBYTES(1)) {}

    Arena(const Arena& copy)
        : Allocator(allocate_proc, (void*)this)
        , capacity(copy.capacity)
        , base(copy.base)
        , current_block(copy.current_block)
        , last_offset(copy.last_offset)
    {}

    Arena& operator=(const Arena& copy)
    {
        current_block = copy.current_block;
        last_offset   = copy.last_offset;
        capacity      = copy.capacity;
        base          = copy.base;
        return *this;
    }

    struct BlockHeader {
        void* prev;
        void* next;
        u64   used;
        u64   count;
    };

    static PROC_ALLOCATOR_ALLOCATE(allocate_proc)
    {
        Arena* self = (Arena*)usr;

        if (self->current_block == nullptr) {
            self->init();
        }

        switch (mode) {
            case AllocatorMode::Reserve: {
                return self->push(new_size);
            } break;

            case AllocatorMode::Resize: {
                return self->copy_or_resize(ptr, prev_size, new_size);
            } break;

            case AllocatorMode::Release: {
                self->try_pop(ptr);
                return 0;
            } break;
        }

        return 0;
    }

    void init() { current_block = create_block(); }

    void deinit()
    {
        while (current_block != nullptr) {
            void* tmp     = current_block;
            current_block = header()->prev;
            base->release(tmp);
        }
        last_offset = 0;
    }

    void reset_to_block(void* block, u64 used = 0, u64 last_offset = 0)
    {
        current_block   = block;
        header()->count = 0;
        header()->used  = used;
        last_offset     = last_offset;
    }

    void reset()
    {
        void* c = current_block;
        void* p = header(c)->prev;

        while (p != nullptr) {
            c = header(c)->prev;
            p = header(c)->prev;
        }

        reset_to_block(c);
    }

    void* create_block()
    {
        void* result = base->reserve(block_size());

        BlockHeader* hdr = (BlockHeader*)result;
        hdr->prev        = nullptr;
        hdr->next        = nullptr;
        hdr->used        = 0;
        hdr->count       = 0;

        return result;
    }

    void create_or_traverse_block()
    {
        if (header()->next != nullptr) {
            switch_to_block(header()->next);
        } else {
            switch_to_block(create_block());
        }
    }

    void switch_to_block(void* block)
    {
        header(current_block)->next = block;
        header(block)->prev         = current_block;
        header(block)->next         = nullptr;
        last_offset                 = 0;
        current_block               = block;
    }

    void* push(u64 size)
    {
        if (size > capacity) return nullptr;

        if ((header()->used + size) > capacity) {
            create_or_traverse_block();
        }

        u8* result  = block_start() + header()->used;
        last_offset = header()->used;
        header()->used += size;

        return result;
    }

    void* copy_or_resize(void* ptr, u64 prev, u64 size)
    {
        if (ptr == nullptr) return push(size);

        u8* last_ptr = block_start() + last_offset;

        if ((last_ptr == ptr) && ((last_offset + size) <= capacity)) {
            header()->used = last_offset + size;
            return last_ptr;
        } else {
            void* new_ptr = push(size);
            if (new_ptr == nullptr) return nullptr;

            memcpy(new_ptr, ptr, prev);
            return new_ptr;
        }
    }

    void try_pop(void* ptr)
    {
        u8* last_ptr = block_start() + last_offset;

        if (last_ptr == ptr) {
            header()->used = last_offset;
            last_offset    = 0;
        }
    }

    BlockHeader* header() const { return (BlockHeader*)(current_block); }
    BlockHeader* header(void* b) const { return (BlockHeader*)(b); }

    u8* block_start() const
    {
        return ((u8*)current_block) + sizeof(BlockHeader);
    }

    u64 block_size() const { return capacity + sizeof(BlockHeader); }

    void*      current_block = nullptr;
    u64        capacity;
    u64        last_offset = 0;
    Allocator* base;
};

template <>
struct MOKLIB_API ArenaSave<ArenaMode::Dynamic> {
    void*                      block;
    u64                        used;
    u64                        last_offset;
    Arena<ArenaMode::Dynamic>* a;

    ArenaSave(Arena<ArenaMode::Dynamic>& arena)
    {
        a           = &arena;
        block       = arena.current_block;
        used        = arena.header()->used;
        last_offset = arena.last_offset;
    }

    ~ArenaSave() { a->reset_to_block(block, used, last_offset); }
};

template <EArenaMode Mode>
static _inline ArenaSave<Mode> _arena_save(Arena<Mode>& a)
{
    ArenaSave<Mode> save(a);
    return std::move(save);
}

#define SAVE_ARENA(arena) \
    auto MJOIN2(_arena_save_, __COUNTER__) = _arena_save(arena);
#define CREATE_SCOPED_ARENA(base, name, capacity)       \
    Arena<ArenaMode::Dynamic> name((base), (capacity)); \
    name.init()