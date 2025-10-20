#include <poc.gui/setup.hpp>

#include <imgui.h>

namespace poc::gui {

auto init() -> void
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();
}

auto uninit() -> void
{
    ImGui::DestroyContext();
}

} // namespace poc::gui
