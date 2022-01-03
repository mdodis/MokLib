#pragma once
/**
 * This is the first time I make heavy use of a language feature to provide
 * a more concenient experience to the user (and myself), so we need this
 * atrocity. Modern C++ - my grass.
 */
#include "Base.h"
#include "Containers/Array.h"
#include "Memory/Base.h"
#include <tuple>
#include <type_traits>

#define MOK_MAX_DELEGATE_INLINE_SIZE 64

namespace DelegateInternals {
    template <bool IsConst, typename T, typename RetVal, typename... Args>
    struct MemberProc;

    template <typename T, typename RetVal, typename... Args>
    struct MemberProc<true, T, RetVal, Args...> {
        using Type = RetVal(T::*)(Args...) const;
    };

    template <typename T, typename RetVal, typename... Args>
    struct MemberProc<false, T, RetVal, Args...> {
        using Type = RetVal(T::*)(Args...);
    };
};

struct IDelegateBase {
    IDelegateBase() = default;
    virtual ~IDelegateBase() noexcept = default;
};

template <typename RetVal, typename... Args>
struct IDelegate : IDelegateBase {
    virtual RetVal call(Args&&... args) = 0;
};

template <bool IsConst, typename T, typename RetVal, typename... Args2>
struct RawDelegate;

template <bool IsConst, typename T, typename RetVal, typename... Args, typename... Args2>
struct RawDelegate<IsConst, T, RetVal(Args...), Args2...> : public IDelegate<RetVal, Args...> {

    using DelegateProc = typename DelegateInternals::MemberProc<IsConst, T, RetVal, Args..., Args2...>::Type;

    T *object;
    DelegateProc proc;
    std::tuple<Args2...> payload;

    RawDelegate(T *object, DelegateProc proc, Args2&&... payload)
        : object(object)
        , proc(proc)
        , payload(std::forward<Args2>(payload)...)
        {}

    RawDelegate(T *object, DelegateProc proc, std::tuple<Args2...>& payload)
        : object(object)
        , proc(proc)
        , payload(payload)
        {}

    virtual RetVal call(Args&&... args) override {
        return _call(std::forward<Args>(args)..., std::index_sequence_for<Args2...>());
    }

    template <std::size_t... Is>
    RetVal _call(Args&&... args, std::index_sequence<Is...>) {
        return (object->*proc)(std::forward<Args>(args)..., std::get<Is>(payload)...);
    }
};

struct DelegateBase {

    u8 buffer[MOK_MAX_DELEGATE_INLINE_SIZE];

    DelegateBase() {}

    DelegateBase(const DelegateBase &other) {
        memcpy(buffer, other.buffer, MOK_MAX_DELEGATE_INLINE_SIZE);
    }

    DelegateBase(DelegateBase &&other) {
        memcpy(buffer, other.buffer, MOK_MAX_DELEGATE_INLINE_SIZE);
    }

    DelegateBase &operator=(const DelegateBase &other) {
        memcpy(buffer, other.buffer, MOK_MAX_DELEGATE_INLINE_SIZE);
        return *this;
    }

    DelegateBase &operator=(DelegateBase &&other) {
        memcpy(buffer, other.buffer, MOK_MAX_DELEGATE_INLINE_SIZE);
        return *this;
    }

    IDelegateBase *get_delegate() const {
        return static_cast<IDelegateBase*>((void*)buffer);
    }

};

template <typename RetVal, typename... Args>
struct Delegate : DelegateBase {

    template <typename T, typename... Args2>
    using MemberProc = typename DelegateInternals::MemberProc<false, T, RetVal, Args..., Args2...>::Type;

    using IDelegateT = IDelegate<RetVal, Args...>;

    template <typename T, typename... Args2>
    static Delegate create_raw(T *object, MemberProc<T, Args2...> proc, Args2... args) {
        Delegate handler;
        handler.bind<RawDelegate<false, T, RetVal(Args...), Args2...>>(object, proc, std::forward<Args2>(args)...);
        return handler;
    }

    template <typename T, typename... Args2>
    void bind_raw(T *object, MemberProc<T, Args2...> proc, Args2&&... args) {
        *this = create_raw<T, Args2...>(object, proc, std::forward<Args2>(args)...);
    }

    template <typename T, typename... Args3>
    void bind(Args3&&...args) {

        new (buffer) T(std::forward<Args3>(args)...);
    }

    RetVal call(Args... args) const {
        return ((IDelegateT*)get_delegate())->call(std::forward<Args>(args)...);
    }

};

/**
 * Handle to delegate (used for multicast delegates)
 */
struct DelegateHandle {
    static u64 Current_Id;
    u64 id;
    constexpr static const u64 Invalid_Id = ~0ull;

    constexpr DelegateHandle() noexcept
        :id(Invalid_Id)
        {}

    explicit DelegateHandle(bool) noexcept
        : id(get_new_id())
        {}

    ~DelegateHandle() noexcept = default;
    DelegateHandle(const DelegateHandle &other) = default;

    DelegateHandle(DelegateHandle &&other) noexcept
        : id(other.id) {
        other.reset();
    }

    DelegateHandle &operator=(DelegateHandle &&other) noexcept {
        id = other.id;
        other.reset();
        return *this;
    }

    operator bool() const noexcept {
        return id != Invalid_Id;
    }

    bool operator==(const DelegateHandle &other) const noexcept {
        return id == other.id;
    }

    void reset() noexcept {
        id = Invalid_Id;
    }

    static u64 get_new_id() {
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
    using MemberProc = typename DelegateInternals::MemberProc<false, T, Args..., Args2...>::Type;

    struct DelegateHandlerPair {
        DelegateHandle handle;
        DelegateT callback;

        DelegateHandlerPair() : handle(false) {}
        DelegateHandlerPair(const DelegateHandle& handle, const DelegateT& callback)
            : handle(handle)
            , callback(callback)
            {}
        DelegateHandlerPair(const DelegateHandle& handle, DelegateT&& callback)
            : handle(handle)
            , callback(std::move(callback))
            {}
    };

    constexpr MulticastDelegate(IAllocator &alloc)
        : locks(0)
        , pairs(&alloc)
        {}
    MulticastDelegate(const MulticastDelegate &other) = default;
    ~MulticastDelegate() = default;

    MulticastDelegate &operator=(const MulticastDelegate &other) = default;
    MulticastDelegate(MulticastDelegate &&other) noexcept
        : pairs(std::move(other.pairs))
        , locks(other.locks)
        {}

    MulticastDelegate &operator=(MulticastDelegate &&other) noexcept {
        pairs = std::move(other.pairs);
        locks = std::move(other.locks);
        return *this;
    }

    DelegateHandle add(DelegateT &&handler) noexcept {
        for (u64 i = 0; i < pairs.size; ++i) {
            if (!pairs[i].handle) {
                pairs[i] = DelegateHandlerPair(DelegateHandle(true), std::move(handler));
                return pairs[i].handle;
            }
        }

        pairs.add(DelegateHandle(true), std::move(handler));
        return pairs.last()->handle;
    }

    template <typename T, typename... Args2>
    DelegateHandle add_raw(T *object, MemberProc<T, Args2...> proc, Args2&&... args) {
        return add(DelegateT::create_raw(object, proc, std::forward<Args2>(args)...));
    }

    void lock() {
        locks++;
    }

    void unlock() {
        ASSERT(locks > 0);
        locks--;
    }

    void broadcast(Args... args) {
        lock();

        for (u64 i = 0; i < pairs.size; ++i) {
            if (pairs[i].handle) {
                pairs[i].callback.call(std::forward<Args>(args)...);
            }
        }

        unlock();
    }

    u32 locks;
    TArray<DelegateHandlerPair> pairs;
    constexpr MulticastDelegate()
        {}

};