#pragma once

#include <poc.core/types.hpp>

namespace poc::gui {

enum class window_flags
{
    none
};

auto begin(string_view name, window_flags flags = window_flags::none) -> bool;
auto begin(string_view name, bool& is_open, window_flags flags = window_flags::none) -> bool;
auto end() -> void;

} // namespace poc::gui
