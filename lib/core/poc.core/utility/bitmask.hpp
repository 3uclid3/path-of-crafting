#pragma once

#include <type_traits>
#include <utility>

namespace poc {

template<typename T>
struct is_bitmask_enum : std::false_type
{
};

template<typename T>
concept bitmask_enum = std::is_enum_v<T> && is_bitmask_enum<T>::value;

template<bitmask_enum T>
[[nodiscard]] constexpr auto operator|(T lhs, T rhs) noexcept -> T;

template<bitmask_enum T>
[[nodiscard]] constexpr auto operator&(T lhs, T rhs) noexcept -> T;

template<bitmask_enum T>
[[nodiscard]] constexpr auto operator^(T lhs, T rhs) noexcept -> T;

template<bitmask_enum T>
[[nodiscard]] constexpr auto operator~(T v) noexcept -> T;

template<bitmask_enum T>
constexpr auto operator|=(T& lhs, T rhs) noexcept -> T&;

template<bitmask_enum T>
constexpr auto operator&=(T& lhs, T rhs) noexcept -> T&;

template<bitmask_enum T>
constexpr auto operator^=(T& lhs, T rhs) noexcept -> T&;

template<bitmask_enum T>
constexpr auto operator|(T lhs, T rhs) noexcept -> T
{
    using U = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<U>(lhs) | static_cast<U>(rhs));
}

template<bitmask_enum T>
constexpr auto operator&(T lhs, T rhs) noexcept -> T
{
    using U = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<U>(lhs) & static_cast<U>(rhs));
}

template<bitmask_enum T>
constexpr auto operator^(T lhs, T rhs) noexcept -> T
{
    using U = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<U>(lhs) ^ static_cast<U>(rhs));
}

template<bitmask_enum T>
constexpr auto operator~(T v) noexcept -> T
{
    using U = std::underlying_type_t<T>;
    return static_cast<T>(~static_cast<U>(v));
}

template<bitmask_enum T>
constexpr auto operator|=(T& lhs, T rhs) noexcept -> T&
{
    lhs = lhs | rhs;
    return lhs;
}

template<bitmask_enum T>
constexpr auto operator&=(T& lhs, T rhs) noexcept -> T&
{
    lhs = lhs & rhs;
    return lhs;
}

template<bitmask_enum T>
constexpr auto operator^=(T& lhs, T rhs) noexcept -> T&
{
    lhs = lhs ^ rhs;
    return lhs;
}

} // namespace poc
