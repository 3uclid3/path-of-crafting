#include <poc.gui/widgets.hpp>

#include <imgui.h>

#include <poc.gui/detail/cstr.hpp>

namespace poc::gui {

auto vtext(string_view fmt, format_args args) -> void
{
    ImGui::TextUnformatted(detail::vformat_cstr(fmt, args));
}

auto selectable(string_view label, bool is_selected, selectable_flags flags) -> bool
{
    return ImGui::Selectable(detail::to_cstr(label), is_selected, static_cast<ImGuiSelectableFlags>(flags));
}

} // namespace poc::gui