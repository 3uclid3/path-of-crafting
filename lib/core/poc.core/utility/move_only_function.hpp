#pragma once

#include <functional>

namespace poc {

#ifdef __cpp_lib_move_only_function
template<typename Signature>
using move_only_function = std::move_only_function<Signature>;
#else
template<typename Signature>
using move_only_function = std::function<Signature>;
#endif

} // namespace poc
