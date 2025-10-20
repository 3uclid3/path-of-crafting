#pragma once

namespace poc::gui::debug {

auto draw_demo_window() -> void;
auto draw_demo_window(bool& is_open) -> void;

auto draw_metrics_window() -> void;
auto draw_metrics_window(bool& is_open) -> void;

auto draw_log_window() -> void;
auto draw_log_window(bool& is_open) -> void;

auto draw_id_stack_window() -> void;
auto draw_id_stack_window(bool& is_open) -> void;

} // namespace poc::gui::debug
