#pragma once

#include <array>
#include <functional>

#include <poc.core/types.hpp>
#include <poc.core/utility/tagged.hpp>

namespace poc::workspace {

class draw_context;

enum class drawing_phase : u8
{
    background,
    main,
    overlay
};

template<typename T>
using drawing_phases = std::array<T, magic_enum::enum_count<drawing_phase>()>;

using draw_priority = tagged<i8, struct draw_priority_tag>;

using drawer_id = string_view;
using drawer = std::function<void(draw_context&)>;

} // namespace poc::workspace
