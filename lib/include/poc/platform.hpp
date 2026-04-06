#pragma once

#include <functional>

namespace poc::platform {

// ----------------------------------------
// Events

auto poll_events() -> void;

// ----------------------------------------
// Console Control Handler

using control_handler = std::function<bool()>;

auto set_console_control_handler(control_handler handler) -> void;

} // namespace poc::platform
