#pragma once
/**
 * This is the first time I make heavy use of a language feature to provide
 * a more concenient experience to the user (and myself), so we need this
 * atrocity. Modern C++ - my grass.
 */
#include <string.h>

#include <tuple>
#include <type_traits>

#include "Base.h"
#include "Config.h"
#include "Containers/Array.h"
#include "Memory/Base.h"

#define MOK_MAX_DELEGATE_INLINE_SIZE 128

namespace DelegateInternals {
    template <bool IsConst, typename T, typename RetVal, typename... Args>
    struct MemberProc;

    template <typename T, typename RetVal, typename... Args>
    struct MemberProc<true, T, RetVal, Args...> {
        using Type = RetVal (T::*)(Args...) const;
    };

    template <typename T, typename RetVal, typename... Args>
    struct MemberProc<false, T, RetVal, Args...> {
        using Type = RetVal (T::*)(Args...);
    };
};  // namespace DelegateInternals

struct IDelegateBase {
    IDelegateBase()                   = default;
    virtual ~IDelegateBase() noexcept = default;
};

template <typename RetVal, typename... Args>
struct IDelegate : IDelegateBase {
    virtual RetVal call(Args&&... args) = 0;
};

/**
 * Static Delegate
 */

template <typename RetVal, typename... Args2>
struct StaticDelegate;

template <typename RetVal, typename... Args, typename... Args2>
struct StaticDelegate<RetVal(Args...), Args2...>
    : public IDelegate<RetVal, Args...> {
    using DelegateProc = RetVal (*)(Args..., Args2...);

    StaticDelegate(DelegateProc proc, Args2&&... payload)
        : proc(proc), payload(std::forward<Args2>(payload)...)
    {}

    StaticDelegate(DelegateProc proc, const std::tuple<Args2...>& payload)
        : proc(proc), payload(payload)
    {}

    virtual RetVal call(Args&&... args) override
    {
        return _call(
            std::forward<Args>(args)...,
            std::index_sequence_for<Args2...>());
    }

    template <std::size_t... Is>
    RetVal _call(Args&&... args, std::index_sequence<Is...>)
    {
        return proc(std::forward<Args>(args)..., std::get<Is>(payload)...);
    }

    DelegateProc         proc;
    std::tuple<Args2...> payload;
};

/**
 * Raw Delegate
 */
template <bool IsConst, typename T, typename RetVal, typename... Args2>
struct RawDelegate;

template <
    bool IsConst,
    typename T,
    typename RetVal,
    typename... Args,
    typename... Args2>
struct RawDelegate<IsConst, T, RetVal(Args...), Args2...>
    : public IDelegate<RetVal, Args...> {
    using DelegateProc = typename DelegateInternals::
        MemberProc<IsConst, T, RetVal, Args..., Args2...>::Type;

    T*                   object;
    DelegateProc         proc;
    std::tuple<Args2...> payload;

    RawDelegate(T* object, DelegateProc proc, Args2&&... payload)
        : object(object), proc(proc), payload(std::forward<Args2>(payload)...)
    {}

    RawDelegate(T* object, DelegateProc proc, std::tuple<Args2...>& payload)
        : object(object), proc(proc), payload(payload)
    {}

    virtual RetVal call(Args&&... args) override
    {
        return _call(
            std::forward<Args>(args)...,
            std::index_sequence_for<Args2...>());
    }

    template <std::size_t... Is>
    RetVal _call(Args&&... args, std::index_sequence<Is...>)
    {
        return (object->*proc)(
            std::forward<Args>(args)...,
            std::get<Is>(payload)...);
    }
};

/**
 * Lambda Delegate
 */
template <typename TLambda, typename RetVal, typename... Args>
struct LambdaDelegate;

template <
    typename TLambda,
    typename RetVal,
    typename... Args,
    typename... Args2>
struct LambdaDelegate<TLambda, RetVal(Args...), Args2...>
    : IDelegate<RetVal, Args...> {
    TLambda              lambda;
    std::tuple<Args2...> payload;

    explicit LambdaDelegate(TLambda&& lambda, Args2&&... payload)
        : lambda(std::forward<TLambda>(lambda))
        , payload(std::forward<Args2>(payload)...)
    {}

    explicit LambdaDelegate(
        const TLambda& lambda, const std::tuple<Args2...>& payload)
        : lambda(lambda), payload(payload)
    {}

    virtual RetVal call(Args&&... args) override
    {
        return _call(
            std::forward<Args>(args)...,
            std::index_sequence_for<Args2...>());
    }

    template <std::size_t... Is>
    RetVal _call(Args&&... args, std::index_sequence<Is...>)
    {
        return (RetVal)((
            lambda)(std::forward<Args>(args)..., std::get<Is>(payload)...));
    }
};

struct DelegateBase {
    u8   buffer[MOK_MAX_DELEGATE_INLINE_SIZE];
    bool is_bound = false;

    DelegateBase() {}

    DelegateBase(const DelegateBase& other)
    {
        memcpy(buffer, other.buffer, MOK_MAX_DELEGATE_INLINE_SIZE);
        is_bound = other.is_bound;
    }

    DelegateBase(DelegateBase&& other)
    {
        memcpy(buffer, other.buffer, MOK_MAX_DELEGATE_INLINE_SIZE);
        is_bound = other.is_bound;
    }

    DelegateBase& operator=(const DelegateBase& other)
    {
        memcpy(buffer, other.buffer, MOK_MAX_DELEGATE_INLINE_SIZE);
        is_bound = other.is_bound;
        return *this;
    }

    DelegateBase& operator=(DelegateBase&& other)
    {
        memcpy(buffer, other.buffer, MOK_MAX_DELEGATE_INLINE_SIZE);
        is_bound = other.is_bound;
        return *this;
    }

    IDelegateBase* get_delegate() const
    {
        return static_cast<IDelegateBase*>((void*)buffer);
    }
};

template <typename RetVal, typename... Args>
struct Delegate : DelegateBase {
    template <typename T, typename... Args2>
    using MemberProc = typename DelegateInternals::
        MemberProc<false, T, RetVal, Args..., Args2...>::Type;

    using IDelegateT = IDelegate<RetVal, Args...>;

    template <typename T, typename... Args2>
    static Delegate create_raw(
        T* object, MemberProc<T, Args2...> proc, Args2... args)
    {
        Delegate handler;
        handler.bind<RawDelegate<false, T, RetVal(Args...), Args2...>>(
            object,
            proc,
            std::forward<Args2>(args)...);
        return handler;
    }

    template <typename... Args2>
    static Delegate create_static(
        RetVal (*proc)(Args..., Args2...), Args2... args)
    {
        Delegate handler;
        handler.bind<StaticDelegate<RetVal(Args...), Args2...>>(
            proc,
            std::forward<Args2>(args)...);
        return handler;
    }

    template <typename TLambda, typename... Args2>
    static Delegate create_lambda(TLambda&& lambda, Args2&&... args)
    {
        Delegate handler;
        handler.bind<LambdaDelegate<TLambda, RetVal(Args...), Args2...>>(
            std::forward<TLambda>(lambda),
            std::forward<Args2>(args)...);
        return handler;
    }

    template <typename T, typename... Args2>
    void bind_raw(T* object, MemberProc<T, Args2...> proc, Args2&&... args)
    {
        *this =
            create_raw<T, Args2...>(object, proc, std::forward<Args2>(args)...);
    }

    template <typename... Args2>
    void bind_static(RetVal (*proc)(Args..., Args2...), Args2&&... args)
    {
        *this = create_static<Args2...>(proc, std::forward<Args2>(args)...);
    }

    template <typename TLambda, typename... Args2>
    void bind_lambda(TLambda&& lambda, Args2&&... args)
    {
        *this = create_lambda(
            std::forward<TLambda>(lambda),
            std::forward<Args2>(args)...);
    }

    template <typename T, typename... Args3>
    void bind(Args3&&... args)
    {
        static_assert(sizeof(T) <= MOK_MAX_DELEGATE_INLINE_SIZE);
        is_bound = true;
        new (buffer) T(std::forward<Args3>(args)...);
    }

    RetVal call(Args... args) const
    {
        return ((IDelegateT*)get_delegate())->call(std::forward<Args>(args)...);
    }

    RetVal call_safe(Args... args) const
    {
        if (is_bound) {
            return call(std::forward<Args>(args)...);
        } else {
            return RetVal();
        }
    }
};

/**
 * Handle to delegate (used for multicast delegates)
 */
struct DelegateHandle {
    MOKLIB_API static u64      Current_Id;
    u64                        id;
    constexpr static const u64 Invalid_Id = ~0ull;

    constexpr DelegateHandle() noexcept : id(Invalid_Id) {}

    explicit DelegateHandle(bool) noexcept : id(get_new_id()) {}

    ~DelegateHandle() noexcept                  = default;
    DelegateHandle(const DelegateHandle& other) = default;

    DelegateHandle(DelegateHandle&& other) noexcept : id(other.id)
    {
        other.reset();
    }

    DelegateHandle& operator=(DelegateHandle&& other) noexcept
    {
        id = other.id;
        other.reset();
        return *this;
    }

    DelegateHandle& operator=(const DelegateHandle& other) noexcept
    {
        id = other.id;
        return *this;
    }

    operator bool() const noexcept { return id != Invalid_Id; }

    bool operator==(const DelegateHandle& other) const noexcept
    {
        return id == other.id;
    }

    void reset() noexcept { id = Invalid_Id; }

    static u64 get_new_id()
    {
        u64 result = DelegateHandle::Current_Id++;
        if (DelegateHandle::Current_Id == Invalid_Id) {
            DelegateHandle::Current_Id = 0;
        }
        return result;
    }
};

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
    constexpr MulticastDelegate() {}
};

#define FORWARD_DELEGATE_RAW(signature, delegate_type, delegate, ret)  \
    template <typename T, typename... Args2>                           \
    signature(                                                         \
        T*                                     object,                 \
        delegate_type::MemberProc<T, Args2...> proc,                   \
        Args2&&... args)                                               \
    {                                                                  \
        delegate.bind_raw(object, proc, std::forward<Args2>(args)...); \
        ret                                                            \
    }

#define FORWARD_DELEGATE_TEMPLATE() template <typename T, typename... Args2>

#define FORWARD_DELEGATE_RAW_SIG(delegate_type) \
    T *object, delegate_type::MemberProc<T, Args2...> proc, Args2 &&... args

#define FORWARD_DELEGATE_RAW_BOD(delegate) \
    delegate.bind_raw(object, proc, std::forward<Args2>(args)...)

#define FORWARD_DELEGATE_LAMBDA_TEMPLATE() \
    template <typename TLambda, typename... Args2>

#define FORWARD_DELEGATE_LAMBDA_SIG(delegate_type) \
    TLambda &&lambda, Args2 &&... args

#define FORWARD_DELEGATE_LAMBDA_CREATE(delegate_type) \
    delegate_type::create_lambda(                     \
        std::forward<TLambda>(lambda),                \
        std::forward<Args2>(args)...)

#define FORWARD_DELEGATE_EXTRA_RAW(                                    \
    signature,                                                         \
    delegate_type,                                                     \
    delegate,                                                          \
    ret,                                                               \
    ...)                                                               \
    template <typename T, typename... Args2>                           \
    signature(                                                         \
        __VA_ARGS__,                                                   \
        T*                                     object,                 \
        delegate_type::MemberProc<T, Args2...> proc,                   \
        Args2&&... args)                                               \
    {                                                                  \
        delegate.bind_raw(object, proc, std::forward<Args2>(args)...); \
        ret                                                            \
    }
