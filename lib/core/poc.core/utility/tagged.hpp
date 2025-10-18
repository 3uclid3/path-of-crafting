#pragma once

#include <compare>
#include <type_traits>
#include <utility>

#include <poc.core/def.hpp>

namespace poc {

template<typename T, typename Tag>
class tagged
{
public:
    using value_type = T;

    // clang-format off
    constexpr tagged() noexcept requires(std::is_default_constructible_v<T>) = default;
    // clang-format on

    constexpr explicit tagged(const T& v) noexcept(std::is_nothrow_copy_constructible_v<T>);
    constexpr explicit tagged(T&& v) noexcept(std::is_nothrow_move_constructible_v<T>);

    constexpr operator const T&() const noexcept;
    constexpr operator T&() noexcept;

    constexpr auto underlying() const noexcept -> const T&;
    constexpr auto underlying() noexcept -> T&;

    friend constexpr auto operator<=>(const tagged&, const tagged&) -> std::strong_ordering = default;

private:
    POC_NO_UNIQUE_ADDRESS T _value{};
};

template<typename T, typename Tag>
constexpr tagged<T, Tag>::tagged(const T& v) noexcept(std::is_nothrow_copy_constructible_v<T>)
    : _value(v)
{
}

template<typename T, typename Tag>
constexpr tagged<T, Tag>::tagged(T&& v) noexcept(std::is_nothrow_move_constructible_v<T>)
    : _value(std::move(v))
{
}

template<typename T, typename Tag>
constexpr tagged<T, Tag>::operator const T&() const noexcept
{
    return _value;
}

template<typename T, typename Tag>
constexpr tagged<T, Tag>::operator T&() noexcept
{
    return _value;
}

template<typename T, typename Tag>
constexpr auto tagged<T, Tag>::underlying() const noexcept -> const T&
{
    return _value;
}

template<typename T, typename Tag>
constexpr auto tagged<T, Tag>::underlying() noexcept -> T&
{
    return _value;
}

} // namespace poc

template<typename T, typename Tag>
struct std::hash<poc::tagged<T, Tag>>
{
    auto operator()(const poc::tagged<T, Tag>& value) const noexcept -> std::size_t
    {
        return std::hash<T>{}(value.underlying());
    }
};
