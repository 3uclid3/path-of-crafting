#include <poc.gui/window.hpp>

#include <imgui.h>

#include <poc.gui/detail/cstr.hpp>

namespace poc::gui {

auto begin(string_view name, window_flags flags) -> bool
{
    return ImGui::Begin(detail::to_cstr(name), nullptr, static_cast<ImGuiWindowFlags>(flags));
}

auto begin(string_view name, bool& is_open, window_flags flags) -> bool
{
    return ImGui::Begin(detail::to_cstr(name), &is_open, static_cast<ImGuiWindowFlags>(flags));
}

auto end() -> void
{
    ImGui::End();
}

} // namespace poc::gui