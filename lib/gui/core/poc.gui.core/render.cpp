#include <poc.gui.core/render.hpp>

#include <imgui.h>

namespace poc::gui {

auto new_frame() -> void
{
    ImGui::NewFrame();
}

auto render() -> void
{
    ImGui::Render();
}

} // namespace poc::gui
