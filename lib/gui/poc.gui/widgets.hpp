#pragma once

#include <poc.core/types.hpp>

namespace poc::gui {

// ---------------------------------------------------------------
// Text

template<typename... Args>
constexpr auto text(format_string<Args...> fmt, Args&&... args) -> void;
auto vtext(string_view fmt, format_args args) -> void;

// ---------------------------------------------------------------
// Selectable

enum class selectable_flags
{
    none
};

auto selectable(string_view label, bool is_selected, selectable_flags flags = selectable_flags::none) -> bool;

// ---------------------------------------------------------------
// Implementation

template<typename... Args>
constexpr auto text(format_string<Args...> fmt, Args&&... args) -> void
{
    vtext(fmt.get(), std::make_format_args(args...));
}

} // namespace poc::gui
