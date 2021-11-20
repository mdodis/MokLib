#include "Sentry.h"
#include "../Host.h"

#if OS_WINDOWS
#include "../WinInc.h"

umm Sentry::push(uint64 size) {
    SYSTEM_INFO info;
    GetSystemInfo(&info);

    uint32 num_pages = (uint32)(size / info.dwPageSize);

    if (num_pages == 0) {
        num_pages = 1;
    }

    num_pages++;

    const uint32 base_size = info.dwPageSize * num_pages;

    const umm base_ptr = (umm)VirtualAlloc(0, base_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    if (!base_ptr) {
        return 0;
    }

    Sentry::Allocation *allocation = allocations.add();
    allocation->base_ptr = base_ptr;
    allocation->base_size = base_size;
    allocation->size = size;

    umm ptr = 0;
    umm guard_page_start = 0;
    if (mode == Sentry::Mode::Overflow) {

        const umm last_page = base_ptr + (num_pages - 1) * info.dwPageSize;
        ptr = last_page - size;

        guard_page_start = last_page;

    } else if (mode == Sentry::Mode::Underflow) {
        ptr = base_ptr + info.dwPageSize;
        guard_page_start = base_ptr;
    }

    allocation->ptr = ptr;

    DWORD old_protect;
    VirtualProtect(guard_page_start, info.dwPageSize, PAGE_NOACCESS, &old_protect);

    return ptr;
}

umm Sentry::resize(umm ptr, uint64 new_size) {

    Allocation *allocation = find_allocation(ptr);
    if (!allocation)
        return push(new_size);


    SYSTEM_INFO info;
    GetSystemInfo(&info);

    uint32 num_pages = (uint32)(new_size / info.dwPageSize);

    if (num_pages == 0) {
        num_pages = 1;
    }

    num_pages++;

    const uint32 base_size = info.dwPageSize * num_pages;

    if (allocation->base_size == base_size) {

        if (mode == Sentry::Mode::Overflow) {
            const umm last_page = allocation->base_ptr + (num_pages - 1) * info.dwPageSize;
            const umm new_ptr = last_page - new_size;
            const umm guard_page_start = last_page;

            DWORD old_protect;
            VirtualProtect(allocation->base_ptr, allocation->base_size, PAGE_READWRITE, &old_protect);

            memmove(new_ptr, allocation->ptr, allocation->size);

            VirtualProtect(guard_page_start, info.dwPageSize, PAGE_NOACCESS, &old_protect);

            allocation->ptr = new_ptr;
            allocation->size = new_size;
            return new_ptr;
        } else if (mode == Sentry::Mode::Underflow) {
            return allocation->ptr;
        } else {
            return 0;
        }

    } else {
        const umm prev_ptr = allocation->ptr;
        const uint64 prev_size = allocation->size;


        umm new_ptr = push(new_size);

        memcpy(new_ptr, prev_ptr, prev_size);

        pop(prev_ptr);
        return new_ptr;
    }


}

void Sentry::pop(umm ptr) {
    int32 allocation_index;
    Allocation *allocation = find_allocation(ptr, &allocation_index);

    if (!allocation)
        return;

    VirtualFree(allocation->base_ptr, 0, MEM_RELEASE);
    allocations.del(allocation_index);
}

#elif OS_LINUX

umm Sentry::push(uint64 size) {
    return 0;
}

umm Sentry::resize(umm ptr, uint64 new_size) {
    return 0;
}

void Sentry::pop(umm ptr) {
    return;
}

#else

#error "Unsupported os for Sentry!"

#endif

Sentry::Allocation *Sentry::find_allocation(umm ptr, int32 *out_index) {
    Allocation *allocation = 0;
    for (int32 i = 0; i < allocations.size; ++i) {
        if (allocations[i].ptr == ptr) {
            allocation = &allocations[i];

            if (out_index) {
                *out_index = i;
            }

            break;
        }
    }

    return allocation;
}

PROC_MEMORY_RESERVE(sentry_reserve) {
    Sentry *sentry = (Sentry*)context;
    return sentry->push(size);
}

PROC_MEMORY_RESIZE(sentry_resize) {
    Sentry *sentry = (Sentry*)context;
    return sentry->resize(ptr, new_size);
}

PROC_MEMORY_RELEASE(sentry_release) {
    Sentry *sentry = (Sentry*)context;
    sentry->pop(ptr);
}

PROC_MEMORY_RELEASE_BASE(sentry_release_base) {
    Sentry *sentry = (Sentry*)context;
    for (int32 i = 0; i < sentry->allocations.size; ++i) {
        sentry->pop(sentry->allocations[i].ptr);
    }

    sentry->allocations.empty();
}

IAllocator Sentry::to_alloc() {

    return IAllocator {
        sentry_reserve,
        sentry_resize,
        sentry_release,
        sentry_release_base,
        this
    };
}

