#pragma once

#include <poc.core/types.hpp>

namespace poc::gui::detail {

auto to_cstr(string_view str) -> const char*;
auto vformat_cstr(string_view fmt, format_args args) -> const char*;

template<class... Args>
auto format_cstr(format_string<Args...> fmt, Args&&... args) -> const char*
{
    return vformat_cstr(fmt.get(), std::make_format_args(std::forward<Args>(args)...));
}

} // namespace poc::gui::detail
