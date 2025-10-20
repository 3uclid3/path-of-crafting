#include <poc.gui/debug.hpp>

#include <imgui.h>

namespace poc::gui::debug {

auto draw_demo_window() -> void
{
    ImGui::ShowDemoWindow();
}

auto draw_demo_window(bool& is_open) -> void
{
    ImGui::ShowDemoWindow(&is_open);
}

auto draw_metrics_window() -> void
{
    ImGui::ShowMetricsWindow();
}

auto draw_metrics_window(bool& is_open) -> void
{
    ImGui::ShowMetricsWindow(&is_open);
}

auto draw_log_window() -> void
{
    ImGui::ShowDebugLogWindow();
}

auto draw_log_window(bool& is_open) -> void
{
    ImGui::ShowDebugLogWindow(&is_open);
}

auto draw_id_stack_window() -> void
{
    ImGui::ShowIDStackToolWindow();
}

auto draw_id_stack_window(bool& is_open) -> void
{
    ImGui::ShowIDStackToolWindow(&is_open);
}

} // namespace poc::gui::debug