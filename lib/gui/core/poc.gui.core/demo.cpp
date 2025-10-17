#include <poc.gui.core/demo.hpp>

#include <imgui.h>

namespace poc::gui {

auto draw_demo() -> void
{
    ImGui::ShowDemoWindow();
}

} // namespace poc::gui