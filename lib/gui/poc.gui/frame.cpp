#include <poc.gui/frame.hpp>

#include <imgui.h>

namespace poc::gui {

auto new_frame() -> void
{
    ImGui::NewFrame();
}

auto end_frame() -> void
{
    ImGui::Render();
}

} // namespace poc::gui
