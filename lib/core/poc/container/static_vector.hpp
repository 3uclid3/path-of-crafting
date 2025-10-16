#pragma once

#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#include <poc/assert.hpp>

namespace poc {

template<typename T, std::size_t Size>
class static_vector
{
public:
    using value_type = T;
    using size_type = std::size_t;

    using iterator = T*;
    using const_iterator = const T*;

    static inline constexpr std::size_t max_size{Size};

    constexpr static_vector() noexcept = default;
    constexpr static_vector(const static_vector& other) noexcept(std::is_nothrow_copy_constructible_v<T>);
    constexpr static_vector(static_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>);
    constexpr ~static_vector() noexcept;

    constexpr auto operator=(const static_vector& other) noexcept(std::is_nothrow_copy_constructible_v<T>) -> static_vector&;
    constexpr auto operator=(static_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>) -> static_vector&;

    constexpr auto size() const noexcept -> size_type;
    constexpr auto capacity() const noexcept -> size_type;
    constexpr auto empty() const noexcept -> bool;
    constexpr auto full() const noexcept -> bool;
    constexpr auto clear() noexcept -> void;

    constexpr auto push_back(const T& value) -> bool;
    constexpr auto push_back(T&& value) -> bool;

    template<typename... Args>
    constexpr auto emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) -> bool;

    constexpr auto pop_back() noexcept -> void;

    constexpr auto operator[](size_type index) noexcept -> T&;
    constexpr auto operator[](size_type index) const noexcept -> const T&;

    constexpr auto front() const noexcept -> const T&;
    constexpr auto front() noexcept -> T&;

    constexpr auto back() const noexcept -> const T&;
    constexpr auto back() noexcept -> T&;

    constexpr auto begin() const noexcept -> const_iterator;
    constexpr auto end() const noexcept -> const_iterator;

    constexpr auto begin() noexcept -> iterator;
    constexpr auto end() noexcept -> iterator;

private:
    constexpr auto ptr() noexcept -> T*;
    constexpr auto ptr() const noexcept -> const T*;

    alignas(T) std::byte _data[sizeof(T) * max_size];
    size_type _size{0};
};

template<typename T, std::size_t Size>
constexpr static_vector<T, Size>::~static_vector() noexcept
{
    clear();
}

template<typename T, std::size_t Size>
constexpr static_vector<T, Size>::static_vector(const static_vector& other) noexcept(std::is_nothrow_copy_constructible_v<T>)
{
    static_assert(std::is_copy_constructible_v<T>, "static_vector requires a copy-constructible value type");

    for (size_type i = 0; i < other._size; ++i)
    {
        std::construct_at(ptr() + i, other[i]);
        ++_size;
    }
}

template<typename T, std::size_t Size>
constexpr static_vector<T, Size>::static_vector(static_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>)
{
    static_assert(std::is_move_constructible_v<T>, "static_vector requires a move-constructible value type");

    for (size_type i = 0; i < other._size; ++i)
    {
        std::construct_at(ptr() + i, std::move(other[i]));
        ++_size;
    }

    other.clear();
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::operator=(const static_vector& other) noexcept(std::is_nothrow_copy_constructible_v<T>) -> static_vector&
{
    static_assert(std::is_copy_constructible_v<T>, "static_vector requires a copy-constructible value type");

    if (this == &other)
    {
        return *this;
    }

    clear();

    for (size_type i = 0; i < other._size; ++i)
    {
        std::construct_at(ptr() + i, other[i]);
        ++_size;
    }

    return *this;
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::operator=(static_vector&& other) noexcept(std::is_nothrow_move_constructible_v<T>) -> static_vector&
{
    static_assert(std::is_move_constructible_v<T>, "static_vector requires a move-constructible value type");

    if (this == &other)
    {
        return *this;
    }

    clear();

    for (size_type i = 0; i < other._size; ++i)
    {
        std::construct_at(ptr() + i, std::move(other[i]));
        ++_size;
    }

    other.clear();

    return *this;
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::size() const noexcept -> size_type
{
    return _size;
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::capacity() const noexcept -> size_type
{
    return Size;
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::empty() const noexcept -> bool
{
    return _size == 0;
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::full() const noexcept -> bool
{
    return _size == max_size;
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::clear() noexcept -> void
{
    if constexpr (!std::is_trivially_destructible_v<T>)
    {
        for (T& item : *this)
        {
            std::destroy_at(&item);
        }
    }

    _size = 0;
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::push_back(const T& value) -> bool
{
    if (full())
    {
        return false;
    }

    T* target = ptr() + _size;
    std::construct_at(target, value);

    ++_size;

    return true;
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::push_back(T&& value) -> bool
{
    if (full())
    {
        return false;
    }

    T* target = ptr() + _size;
    std::construct_at(target, std::move(value));

    ++_size;

    return true;
}

template<typename T, std::size_t Size>
template<typename... Args>
constexpr auto static_vector<T, Size>::emplace_back(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) -> bool
{
    if (full())
    {
        return false;
    }

    T* target = ptr() + _size;
    std::construct_at(target, std::forward<Args>(args)...);

    ++_size;

    return true;
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::pop_back() noexcept -> void
{
    if (empty())
    {
        return;
    }

    --_size;

    if constexpr (!std::is_trivially_destructible_v<T>)
    {
        T* target = ptr() + _size;
        std::destroy_at(target);
    }
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::operator[](size_type index) noexcept -> T&
{
    POC_ASSERT(index < _size);
    return *(std::launder(ptr() + index));
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::operator[](size_type index) const noexcept -> const T&
{
    POC_ASSERT(index < _size);
    return *(std::launder(ptr() + index));
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::front() const noexcept -> const T&
{
    return (*this)[0];
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::front() noexcept -> T&
{
    return (*this)[0];
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::back() const noexcept -> const T&
{
    return (*this)[_size - 1];
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::back() noexcept -> T&
{
    return (*this)[_size - 1];
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::begin() const noexcept -> const_iterator
{
    const auto* base = ptr();
    return _size == 0 ? base : std::launder(base);
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::end() const noexcept -> const_iterator
{
    return begin() + _size;
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::begin() noexcept -> iterator
{
    auto* base = ptr();
    return _size == 0 ? base : std::launder(base);
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::end() noexcept -> iterator
{
    return begin() + _size;
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::ptr() noexcept -> T*
{
    return reinterpret_cast<T*>(_data);
}

template<typename T, std::size_t Size>
constexpr auto static_vector<T, Size>::ptr() const noexcept -> const T*
{
    return reinterpret_cast<const T*>(_data);
}

} // namespace poc
