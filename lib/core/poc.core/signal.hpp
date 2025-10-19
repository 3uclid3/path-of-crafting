#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>

#include <poc.core/assert.hpp>
#include <poc.core/def.hpp>

namespace poc {

// basic_inplace_signal<InplaceSize, Allocator, Args...>
//  - InplaceSize: inplace slot capacity before one-time heap migration
//  - Allocator: e.g. std::allocator<std::byte>
//  - Args...: call signature is void(Args...)
//
// Notes: Cannot be moved if there are connected slots.
//        Slot storage is fixed-size (48 bytes) and must be large enough for callables.
template<std::size_t InplaceSize, typename Allocator, typename... Args>
class basic_inplace_signal;

template<typename Signal>
class basic_signal_connection;

template<typename Signal>
class basic_signal_connector;

template<typename Signal>
class basic_signal_scoped_block;

template<typename... Args>
using signal = basic_inplace_signal<0, std::allocator<std::byte>, Args...>;

template<std::size_t InplaceSize, typename... Args>
using inplace_signal = basic_inplace_signal<InplaceSize, std::allocator<std::byte>, Args...>;

template<typename T, std::size_t Size>
struct basic_inplace_signal_buffer
{
    static constexpr std::size_t size = Size;

    constexpr auto data() const noexcept -> const T*;
    constexpr auto data() noexcept -> T*;

    std::array<T, size> storage{};
};

template<typename T>
struct basic_inplace_signal_buffer<T, 0>
{
    static constexpr std::size_t size = 0;

    constexpr auto data() const noexcept -> const T*;
    constexpr auto data() noexcept -> T*;
};

template<std::size_t InplaceSize, typename Allocator, typename... Args>
class basic_inplace_signal
{
public:
    using allocator_type = typename std::allocator_traits<Allocator>::template rebind_alloc<std::byte>;
    using connection_type = basic_signal_connection<basic_inplace_signal>;
    using connector_type = basic_signal_connector<basic_inplace_signal>;
    using scoped_block_type = basic_signal_scoped_block<basic_inplace_signal>;

    using size_type = std::size_t;

    static constexpr std::size_t inplace_size{InplaceSize};
    static constexpr std::size_t inplace_slot_storage_size{48};

public:
    basic_inplace_signal() noexcept(std::is_nothrow_default_constructible_v<allocator_type>) = default;
    explicit basic_inplace_signal(const allocator_type& alloc) noexcept;

    ~basic_inplace_signal();

    basic_inplace_signal(basic_inplace_signal&& other) noexcept;
    auto operator=(basic_inplace_signal&& other) noexcept -> basic_inplace_signal&;

    basic_inplace_signal(const basic_inplace_signal&) = delete;
    auto operator=(const basic_inplace_signal&) -> basic_inplace_signal& = delete;

    [[nodiscard]] auto connector() noexcept -> connector_type;
    auto disconnect_all() noexcept -> void;

    auto emit(const std::decay_t<Args>&... args) -> void;
    auto operator()(const std::decay_t<Args>&... args) -> void;

    auto reset() noexcept -> void;     // disconnect all and deallocate heap if any
    auto reserve(size_type n) -> void; // no-op before migration when InplaceSize sufficient

    auto empty() const noexcept -> bool;
    auto size() const noexcept -> size_type;     // connected slots
    auto capacity() const noexcept -> size_type; // InplaceSize or heap capacity if migrated
    auto is_heap() const noexcept -> bool;

    auto blocked() const noexcept -> bool;
    auto block() noexcept -> scoped_block_type;

    auto get_allocator() const noexcept -> allocator_type;

private:
    // type-erased in-place callable per slot
    struct slot_vtable
    {
        using invoke_type = void (*)(void*, const std::decay_t<Args>&...);
        using destroy_type = void (*)(void*);
        using move_type = void (*)(void* dst, void* src);

        invoke_type invoke = nullptr;
        destroy_type destroy = nullptr;
        move_type move = nullptr;
    };

    struct slot
    {
        const slot_vtable* vtable{nullptr};
        alignas(std::max_align_t) std::array<std::byte, inplace_slot_storage_size> storage{};
    };

    template<typename Fn>
    static auto slot_invoke(void* storage, const std::decay_t<Args>&... call_args) -> void;

    template<typename Fn>
    static auto slot_destroy(void* storage) -> void;

    template<typename Fn>
    static auto slot_move(void* dst, void* src) -> void;

    template<typename Fn>
    static auto slot_vtable_for() -> const slot_vtable*;

    // connection helpers
    template<typename F>
    [[nodiscard]] auto connect_impl(F&& f) -> connection_type;

    auto drop(size_type index) noexcept -> void;
    auto used(size_type index) const noexcept -> bool;

    // storage management
    auto ensure_capacity(size_type min_capacity) -> void;
    auto allocation_size(size_type capacity) const noexcept -> size_type;
    auto grow_heap(size_type min_new_cap) -> void;
    auto compact_tail() noexcept -> void;
    auto begin_block() noexcept -> void;
    auto end_block() noexcept -> void;

    auto can_move() const noexcept -> bool;
    auto move_from(basic_inplace_signal& other) noexcept -> void;

private:
    using inplace_slots_type = basic_inplace_signal_buffer<slot, inplace_size>;
    using inplace_useds_type = basic_inplace_signal_buffer<bool, inplace_size>;

    // allocate 1 buffer for slots and useds
    POC_NO_UNIQUE_ADDRESS allocator_type _allocator{};

    // inplace storage
    POC_NO_UNIQUE_ADDRESS inplace_slots_type _inplace_slots{};
    POC_NO_UNIQUE_ADDRESS inplace_useds_type _inplace_useds{};

    // current storage (inplace or heap)
    slot* _slots{_inplace_slots.data()};
    bool* _useds{_inplace_useds.data()};

    size_type _added_size{0};     // includes disconnected slots
    size_type _connected_size{0}; // currently connected slots
    size_type _heap_capacity{0};  // 0 if using inplace storage
    size_type _block_count{0};

    friend connection_type;
    friend connector_type;
    friend scoped_block_type;

    static_assert((!std::is_rvalue_reference_v<Args> && ...), "basic_inplace_signal does not support T&& listener parameter types");
};

template<std::size_t InplaceSize, typename Allocator, typename... Args>
class basic_signal_connection<basic_inplace_signal<InplaceSize, Allocator, Args...>>
{
public:
    using signal_type = basic_inplace_signal<InplaceSize, Allocator, Args...>;
    using size_type = typename signal_type::size_type;

    basic_signal_connection() noexcept = default;
    ~basic_signal_connection();

    basic_signal_connection(basic_signal_connection&&) noexcept;
    auto operator=(basic_signal_connection&&) noexcept -> basic_signal_connection&;

    basic_signal_connection(const basic_signal_connection&) = delete;
    auto operator=(const basic_signal_connection&) -> basic_signal_connection& = delete;

    explicit operator bool() const noexcept;
    auto is_connected() const noexcept -> bool;
    auto disconnect() noexcept -> void;
    auto release() noexcept -> void;          // give up ownership without disconnecting
    auto index() const noexcept -> size_type; // stable slot index or npos

private:
    static constexpr auto npos = static_cast<size_type>(-1);

    basic_signal_connection(signal_type* signal, size_type index) noexcept;

    signal_type* _signal{nullptr};
    size_type _index{npos};

    friend signal_type;
};

template<std::size_t InplaceSize, typename Allocator, typename... Args>
class basic_signal_connector<basic_inplace_signal<InplaceSize, Allocator, Args...>>
{
public:
    using signal_type = basic_inplace_signal<InplaceSize, Allocator, Args...>;
    using connection_type = typename signal_type::connection_type;

    basic_signal_connector() noexcept = default;

    basic_signal_connector(basic_signal_connector&&) noexcept;
    auto operator=(basic_signal_connector&&) noexcept -> basic_signal_connector&;

    basic_signal_connector(const basic_signal_connector&) = delete;
    auto operator=(const basic_signal_connector&) -> basic_signal_connector& = delete;

    template<typename F>
    [[nodiscard]] auto connect(F&& f) -> connection_type;

    template<typename T>
    [[nodiscard]] auto connect(T* obj, auto (T::*mf)(Args...)) -> connection_type;

    template<typename T>
    [[nodiscard]] auto connect(const T* obj, auto (T::*mf)(Args...) const) -> connection_type;

    template<typename T>
    [[nodiscard]] auto connect(T& obj, auto (T::*mf)(Args...)) -> connection_type;

    template<typename T>
    [[nodiscard]] auto connect(const T& obj, auto (T::*mf)(Args...) const) -> connection_type;

private:
    explicit basic_signal_connector(signal_type* signal) noexcept;

    signal_type* _signal{nullptr};

    friend signal_type;
};

template<std::size_t InplaceSize, typename Allocator, typename... Args>
class basic_signal_scoped_block<basic_inplace_signal<InplaceSize, Allocator, Args...>>
{
public:
    using signal_type = basic_inplace_signal<InplaceSize, Allocator, Args...>;

    basic_signal_scoped_block() noexcept = default;
    ~basic_signal_scoped_block();

    basic_signal_scoped_block(basic_signal_scoped_block&& other) noexcept;
    auto operator=(basic_signal_scoped_block&& other) noexcept -> basic_signal_scoped_block&;

    basic_signal_scoped_block(const basic_signal_scoped_block&) = delete;
    auto operator=(const basic_signal_scoped_block&) -> basic_signal_scoped_block& = delete;

private:
    explicit basic_signal_scoped_block(signal_type* signal) noexcept;

    auto release() noexcept -> void;

    signal_type* _signal = nullptr;

    friend signal_type;
};

template<typename T, std::size_t Size>
constexpr auto basic_inplace_signal_buffer<T, Size>::data() const noexcept -> const T*
{
    return storage.data();
}

template<typename T, std::size_t Size>
constexpr auto basic_inplace_signal_buffer<T, Size>::data() noexcept -> T*
{
    return storage.data();
}

template<typename T>
constexpr auto basic_inplace_signal_buffer<T, 0>::data() const noexcept -> const T*
{
    return nullptr;
}

template<typename T>
constexpr auto basic_inplace_signal_buffer<T, 0>::data() noexcept -> T*
{
    return nullptr;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
basic_signal_connection<basic_inplace_signal<InplaceSize, Allocator, Args...>>::~basic_signal_connection()
{
    disconnect();
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
basic_signal_connection<basic_inplace_signal<InplaceSize, Allocator, Args...>>::basic_signal_connection(basic_signal_connection&& other) noexcept
    : _signal(std::exchange(other._signal, nullptr))
    , _index(std::exchange(other._index, npos))
{
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_signal_connection<basic_inplace_signal<InplaceSize, Allocator, Args...>>::operator=(basic_signal_connection&& other) noexcept -> basic_signal_connection&
{
    if (this != &other)
    {
        disconnect();

        _signal = std::exchange(other._signal, nullptr);
        _index = std::exchange(other._index, npos);
    }

    return *this;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
basic_signal_connection<basic_inplace_signal<InplaceSize, Allocator, Args...>>::operator bool() const noexcept
{
    return is_connected();
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_signal_connection<basic_inplace_signal<InplaceSize, Allocator, Args...>>::is_connected() const noexcept -> bool
{
    return _signal != nullptr && _index != npos && _signal->used(_index);
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_signal_connection<basic_inplace_signal<InplaceSize, Allocator, Args...>>::disconnect() noexcept -> void
{
    if (_signal != nullptr)
    {
        if (_index != npos && _signal->used(_index))
        {
            _signal->drop(_index);
        }

        release();
    }
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_signal_connection<basic_inplace_signal<InplaceSize, Allocator, Args...>>::release() noexcept -> void
{
    _signal = nullptr;
    _index = npos;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_signal_connection<basic_inplace_signal<InplaceSize, Allocator, Args...>>::index() const noexcept -> size_type
{
    return is_connected() ? _index : npos;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
basic_signal_connection<basic_inplace_signal<InplaceSize, Allocator, Args...>>::basic_signal_connection(signal_type* signal, size_type index) noexcept
    : _signal(signal)
    , _index(index)
{
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
template<typename F>
auto basic_signal_connector<basic_inplace_signal<InplaceSize, Allocator, Args...>>::connect(F&& f) -> connection_type
{
    return _signal->connect_impl(std::forward<F>(f));
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
template<typename T>
auto basic_signal_connector<basic_inplace_signal<InplaceSize, Allocator, Args...>>::connect(T* obj, auto (T::*mf)(Args...)) -> connection_type
{
    POC_ASSERT(obj != nullptr);

    return connect([obj, mf](Args... call_args) {
        std::invoke(mf, obj, call_args...);
    });
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
template<typename T>
auto basic_signal_connector<basic_inplace_signal<InplaceSize, Allocator, Args...>>::connect(const T* obj, auto (T::*mf)(Args...) const) -> connection_type
{
    POC_ASSERT(obj != nullptr);

    return connect([obj, mf](Args... call_args) {
        std::invoke(mf, obj, call_args...);
    });
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
template<typename T>
auto basic_signal_connector<basic_inplace_signal<InplaceSize, Allocator, Args...>>::connect(T& obj, auto (T::*mf)(Args...)) -> connection_type
{
    POC_ASSERT(obj != nullptr);

    return connect([&obj, mf](Args... call_args) {
        std::invoke(mf, obj, call_args...);
    });
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
template<typename T>
auto basic_signal_connector<basic_inplace_signal<InplaceSize, Allocator, Args...>>::connect(const T& obj, auto (T::*mf)(Args...) const) -> connection_type
{
    POC_ASSERT(obj != nullptr);

    return connect([&obj, mf](Args... call_args) {
        std::invoke(mf, obj, call_args...);
    });
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
basic_signal_connector<basic_inplace_signal<InplaceSize, Allocator, Args...>>::basic_signal_connector(signal_type* signal) noexcept
    : _signal(signal)
{
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
basic_signal_scoped_block<basic_inplace_signal<InplaceSize, Allocator, Args...>>::~basic_signal_scoped_block()
{
    release();
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
basic_signal_scoped_block<basic_inplace_signal<InplaceSize, Allocator, Args...>>::basic_signal_scoped_block(basic_signal_scoped_block&& other) noexcept
    : _signal(std::exchange(other._signal, nullptr))
{
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_signal_scoped_block<basic_inplace_signal<InplaceSize, Allocator, Args...>>::operator=(basic_signal_scoped_block&& other) noexcept -> basic_signal_scoped_block&
{
    if (this != &other)
    {
        release();
        _signal = std::exchange(other._signal, nullptr);
    }

    return *this;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
basic_signal_scoped_block<basic_inplace_signal<InplaceSize, Allocator, Args...>>::basic_signal_scoped_block(signal_type* signal) noexcept
    : _signal(signal)
{
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_signal_scoped_block<basic_inplace_signal<InplaceSize, Allocator, Args...>>::release() noexcept -> void
{
    if (_signal != nullptr)
    {
        _signal->end_block();
        _signal = nullptr;
    }
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
basic_inplace_signal<InplaceSize, Allocator, Args...>::basic_inplace_signal(const allocator_type& alloc) noexcept
    : _allocator(alloc)
{
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
basic_inplace_signal<InplaceSize, Allocator, Args...>::~basic_inplace_signal()
{
    POC_ASSERT(_connected_size == 0);

    reset();
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
basic_inplace_signal<InplaceSize, Allocator, Args...>::basic_inplace_signal(basic_inplace_signal&& other) noexcept
{
    move_from(other);
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::operator=(basic_inplace_signal&& other) noexcept -> basic_inplace_signal&
{
    if (this != &other)
    {
        reset();

        move_from(other);
    }

    return *this;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::connector() noexcept -> connector_type
{
    return connector_type(this);
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::disconnect_all() noexcept -> void
{
    for (size_type i = 0; i < _added_size; ++i)
    {
        if (_useds[i])
        {
            if (auto& slot = _slots[i]; slot.vtable != nullptr)
            {
                slot.vtable->destroy(&slot.storage);
                slot.vtable = nullptr;
            }
            _useds[i] = false;
        }
    }

    _connected_size = 0;
    _added_size = 0;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::emit(const std::decay_t<Args>&... args) -> void
{
    if (empty() || blocked())
    {
        return;
    }

    for (size_type i = 0, size = _added_size; i < size; ++i)
    {
        if (used(i))
        {
            auto& slot = _slots[i];
            slot.vtable->invoke(&slot.storage, args...);
        }
    }
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::operator()(const std::decay_t<Args>&... args) -> void
{
    emit(args...);
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::reset() noexcept -> void
{
    disconnect_all();

    if (is_heap())
    {
        std::destroy_n(_slots, _heap_capacity);
        auto raw = reinterpret_cast<std::byte*>(_slots);
        std::allocator_traits<allocator_type>::deallocate(_allocator, raw, allocation_size(_heap_capacity));
    }

    _heap_capacity = 0;
    _slots = _inplace_slots.data();
    _useds = _inplace_useds.data();

    if constexpr (inplace_size > 0)
    {
        std::fill(_useds, _useds + inplace_size, false);

        for (size_type i = 0; i < inplace_size; ++i)
        {
            _slots[i].vtable = nullptr;
        }
    }

    _added_size = 0;
    _connected_size = 0;
    _block_count = 0;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::reserve(size_type n) -> void
{
    ensure_capacity(n);
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::empty() const noexcept -> bool
{
    return _connected_size == 0;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::size() const noexcept -> size_type
{
    return _connected_size;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::capacity() const noexcept -> size_type
{
    return is_heap() ? _heap_capacity : inplace_size;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::is_heap() const noexcept -> bool
{
    return _heap_capacity > 0;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::blocked() const noexcept -> bool
{
    return _block_count > 0;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::block() noexcept -> scoped_block_type
{
    begin_block();
    return scoped_block_type(this);
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::get_allocator() const noexcept -> allocator_type
{
    return _allocator;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
template<typename Fn>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::slot_invoke(void* storage, const std::decay_t<Args>&... call_args) -> void
{
    auto* fn = reinterpret_cast<Fn*>(storage);
    std::invoke(*fn, call_args...);
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
template<typename Fn>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::slot_destroy(void* storage) -> void
{
    std::destroy_at(reinterpret_cast<Fn*>(storage));
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
template<typename Fn>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::slot_move(void* dst, void* src) -> void
{
    auto* src_fn = reinterpret_cast<Fn*>(src);
    new (dst) Fn(std::move(*src_fn));
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
template<typename Fn>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::slot_vtable_for() -> const slot_vtable*
{
    static const slot_vtable table{&slot_invoke<Fn>, &slot_destroy<Fn>, &slot_move<Fn>};
    return &table;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
template<typename F>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::connect_impl(F&& f) -> connection_type
{
    using Fn = std::decay_t<F>;

    static_assert(std::is_invocable_v<Fn&, Args...>, "callable must accept signal argument types");
    static_assert(sizeof(Fn) <= inplace_slot_storage_size, "callable too large for slot storage");
    static_assert(alignof(Fn) <= alignof(std::max_align_t), "callable alignment exceeds slot storage alignment");

    size_type index = 0;
    for (; index < _added_size; ++index)
    {
        if (!_useds[index])
        {
            break;
        }
    }

    ensure_capacity(index + 1);

    auto& slot = _slots[index];
    const auto* vtable = slot_vtable_for<Fn>();

    new (&slot.storage) Fn(std::forward<F>(f));
    slot.vtable = vtable;
    _useds[index] = true;
    ++_connected_size;
    _added_size = std::max(_added_size, index + 1);

    return connection_type(this, index);
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::drop(size_type index) noexcept -> void
{
    POC_ASSERT(used(index));
    POC_ASSERT(_connected_size > 0);

    auto& slot = _slots[index];
    if (slot.vtable != nullptr)
    {
        slot.vtable->destroy(&slot.storage);
        slot.vtable = nullptr;
    }

    _useds[index] = false;
    --_connected_size;
    compact_tail();
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::used(size_type index) const noexcept -> bool
{
    return _useds != nullptr && index < _added_size && _useds[index];
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::ensure_capacity(size_type min_capacity) -> void
{
    if (min_capacity == 0 || min_capacity <= capacity())
    {
        return;
    }

    if (!is_heap() && min_capacity <= inplace_size)
    {
        return;
    }

    grow_heap(min_capacity);
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::allocation_size(size_type cap) const noexcept -> size_type
{
    const size_type slot_array_size = cap * sizeof(slot);
    const size_type bool_array_offset = (slot_array_size + alignof(bool) - 1) & ~(alignof(bool) - 1);

    return bool_array_offset + cap * sizeof(bool);
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::grow_heap(size_type min_new_cap) -> void
{
    if (min_new_cap <= _heap_capacity)
    {
        return;
    }

    size_type growth_base = _heap_capacity > 0 ? _heap_capacity : inplace_size;
    size_type new_cap = std::max<size_type>(min_new_cap, growth_base + (growth_base >> 1) + 1); // 1.5x + 1 growth
    if (new_cap == 0)
    {
        new_cap = 1;
    }

    // allocate new storage for slots and useds
    const size_type slot_bytes = new_cap * sizeof(slot);
    const size_type total_bytes = allocation_size(new_cap);

    std::byte* raw_mem = std::allocator_traits<allocator_type>::allocate(_allocator, total_bytes);

    auto new_slots = reinterpret_cast<slot*>(raw_mem);
    auto new_useds = reinterpret_cast<bool*>(raw_mem + slot_bytes);

    std::uninitialized_value_construct_n(new_slots, new_cap);
    std::fill_n(new_useds, new_cap, false);

    slot* old_slots = _slots;
    bool* old_useds = _useds;
    const bool was_heap = is_heap();
    const size_type old_heap_capacity = _heap_capacity;
    const size_type limit = _added_size;

    for (size_type i = 0; i < limit; ++i)
    {
        const bool slot_used = old_useds != nullptr && old_useds[i];
        new_useds[i] = slot_used;

        if (slot_used)
        {
            auto& src_slot = old_slots[i];
            auto& dst_slot = new_slots[i];
            dst_slot.vtable = src_slot.vtable;
            dst_slot.vtable->move(&dst_slot.storage, &src_slot.storage);
        }
    }

    for (size_type i = 0; i < limit; ++i)
    {
        if (old_useds != nullptr && old_useds[i])
        {
            auto& src_slot = old_slots[i];
            if (src_slot.vtable != nullptr)
            {
                src_slot.vtable->destroy(&src_slot.storage);
                src_slot.vtable = nullptr;
            }
            old_useds[i] = false;
        }
    }

    if (was_heap)
    {
        std::destroy_n(old_slots, old_heap_capacity);
        auto old_raw_mem = reinterpret_cast<std::byte*>(old_slots);
        std::allocator_traits<allocator_type>::deallocate(_allocator, old_raw_mem, allocation_size(old_heap_capacity));
    }

    _slots = new_slots;
    _useds = new_useds;
    _heap_capacity = new_cap;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::compact_tail() noexcept -> void
{
    while (_added_size > 0 && !_useds[_added_size - 1])
    {
        --_added_size;
    }
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::begin_block() noexcept -> void
{
    ++_block_count;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::end_block() noexcept -> void
{
    POC_ASSERT(_block_count > 0);
    --_block_count;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::can_move() const noexcept -> bool
{
    return _connected_size == 0 && _block_count == 0;
}

template<std::size_t InplaceSize, typename Allocator, typename... Args>
auto basic_inplace_signal<InplaceSize, Allocator, Args...>::move_from(basic_inplace_signal& other) noexcept -> void
{
    POC_ASSERT(this != &other);
    POC_ASSERT(other.can_move());

    _allocator = std::move(other._allocator);

    if constexpr (inplace_size == 0)
    {
        _slots = std::exchange(other._slots, nullptr);
        _useds = std::exchange(other._useds, nullptr);
    }
    else if (other.is_heap())
    {
        _slots = std::exchange(other._slots, _inplace_slots.data());
        _useds = std::exchange(other._useds, _inplace_useds.data());
    }
    else
    {
        _slots = _inplace_slots.data();
        _useds = _inplace_useds.data();
    }

    _added_size = std::exchange(other._added_size, 0);
    _connected_size = std::exchange(other._connected_size, 0);
    _heap_capacity = std::exchange(other._heap_capacity, 0);
    _block_count = std::exchange(other._block_count, 0);
}

} // namespace poc
