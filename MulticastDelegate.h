#pragma once
#include "Containers/Array.h"
#include "Delegates.h"
#include "Memory/Base.h"

template <typename... Args>
struct MulticastDelegate : DelegateBase {
    using DelegateT = Delegate<void, Args...>;
    template <typename T, typename... Args2>
    using MemberProc = typename DelegateInternals::
        MemberProc<false, T, void, Args..., Args2...>::Type;

    struct DelegateHandlerPair {
        DelegateHandle handle;
        DelegateT      callback;

        DelegateHandlerPair() : handle(false) {}
        DelegateHandlerPair(
            const DelegateHandle& handle, const DelegateT& callback)
            : handle(handle), callback(callback)
        {}
        DelegateHandlerPair(const DelegateHandle& handle, DelegateT&& callback)
            : handle(handle), callback(std::move(callback))
        {}

        DelegateHandlerPair& operator=(
            const DelegateHandlerPair& other) noexcept
        {
            handle   = other.handle;
            callback = other.callback;
            return *this;
        }
    };

    constexpr MulticastDelegate() : locks(0), pairs(&System_Allocator) {}
    constexpr MulticastDelegate(IAllocator& alloc) : locks(0), pairs(&alloc) {}
    MulticastDelegate(const MulticastDelegate& other) = default;
    ~MulticastDelegate()                              = default;

    MulticastDelegate& operator=(const MulticastDelegate& other) = default;
    MulticastDelegate(MulticastDelegate&& other) noexcept
        : pairs(std::move(other.pairs)), locks(other.locks)
    {}

    MulticastDelegate& operator=(MulticastDelegate&& other) noexcept
    {
        pairs = std::move(other.pairs);
        locks = std::move(other.locks);
        return *this;
    }

    DelegateHandle add(DelegateT&& handler) noexcept
    {
        for (i32 i = 0; i < pairs.size; ++i) {
            if (!pairs[i].handle) {
                pairs[i] = DelegateHandlerPair(
                    DelegateHandle(true),
                    std::move(handler));
                return pairs[i].handle;
            }
        }

        DelegateHandlerPair pair(DelegateHandle(true), std::move(handler));
        pairs.add(pair);
        return pairs.last()->handle;
    }

    template <typename T, typename... Args2>
    DelegateHandle add_raw(
        T* object, MemberProc<T, Args2...> proc, Args2&&... args)
    {
        return add(
            DelegateT::create_raw(object, proc, std::forward<Args2>(args)...));
    }

    template <typename... Args2>
    DelegateHandle add_static(void (*proc)(Args..., Args2...), Args2&&... args)
    {
        return add(
            DelegateT::create_static(proc, std::forward<Args2>(args)...));
    }

    void lock() { locks++; }

    void unlock()
    {
        ASSERT(locks > 0);
        locks--;
    }

    void broadcast(Args... args)
    {
        lock();

        for (i32 i = 0; i < pairs.size; ++i) {
            if (pairs[i].handle) {
                pairs[i].callback.call(std::forward<Args>(args)...);
            }
        }

        unlock();
    }

    u32                         locks;
    TArray<DelegateHandlerPair> pairs;
};
