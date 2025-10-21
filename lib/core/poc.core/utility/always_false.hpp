#pragma once

#include <type_traits>

namespace poc {

template<typename T>
struct always_false : std::integral_constant<bool, false>
{
};

template<typename T>
inline constexpr bool always_false_v = always_false<T>::value;

} // namespace poc
