#pragma once

#include <array>
#include <functional>

#include <poc.core/types.hpp>
#include <poc.core/utility/crc32.hpp>
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

using drawer_id = tagged<u32, struct drawer_id_tag>;
using drawer = std::function<void(draw_context&)>;

consteval drawer_id make_drawer_id(string_view str) { return drawer_id{compile_crc32(str)}; }

} // namespace poc::workspace
