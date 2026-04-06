#include <poc/platform.hpp>

#define NOMINMAX
#include <windows.h>

namespace poc::platform {

namespace console {

static control_handler g_control_handler;

auto control_handler_hook([[maybe_unused]] DWORD ctrl_type) -> BOOL
{
    return g_control_handler();
}

} // namespace console

auto poll_events() -> void
{
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

auto set_console_control_handler(control_handler handler) -> void
{
    console::g_control_handler = std::move(handler);
    ::SetConsoleCtrlHandler(console::control_handler_hook, TRUE);
}

} // namespace poc::platform
