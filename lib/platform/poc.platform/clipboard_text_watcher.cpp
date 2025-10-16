#include <poc.platform/clipboard_text_watcher.hpp>

#include <SDL3/SDL.h>

#ifdef SDL_PLATFORM_WIN32

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#include <poc.platform/log.hpp>
#include <poc.platform/window.hpp>

namespace poc::platform {

namespace {

static auto window_message_hook(void* userdata, MSG* msg) -> bool
{
    clipboard_text_watcher::text_changed_callback* watcher_callback = static_cast<clipboard_text_watcher::text_changed_callback*>(userdata);

    if (msg->message == WM_CLIPBOARDUPDATE)
    {
        if (char* text = SDL_GetClipboardText())
        {
            (*watcher_callback)(text);
            SDL_free(text);
        }
        else
        {
            (*watcher_callback)("");
        }
    }

    // don’t consume
    return true;
}

static auto get_hwnd(SDL_Window* window) -> HWND
{
    SDL_PropertiesID pid = SDL_GetWindowProperties(window);
    return static_cast<HWND>(SDL_GetPointerProperty(pid, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
}

static auto watch_clipboard_changed(SDL_Window* window, clipboard_text_watcher::text_changed_callback& watcher_callback) -> void
{
    HWND hwnd = get_hwnd(window);

    if (!hwnd)
    {
        POC_PLATFORM_WARN("Failed to get HWND for clipboard events");
        return;
    }

    if (!AddClipboardFormatListener(hwnd))
    {
        POC_PLATFORM_WARN("AddClipboardFormatListener failed: {}", GetLastError());
        return;
    }

    SDL_SetWindowsMessageHook(window_message_hook, &watcher_callback);
}

auto stop_watch_clipboard_changed(SDL_Window* window) -> void
{
    SDL_SetWindowsMessageHook(nullptr, nullptr);

    if (HWND hwnd = get_hwnd(window))
    {
        if (!RemoveClipboardFormatListener(hwnd))
        {
            POC_PLATFORM_WARN("RemoveClipboardFormatListener failed: {}", GetLastError());
        }
    }
    else
    {
        POC_PLATFORM_WARN("Failed to get HWND for clipboard events");
    }
}

} // namespace

clipboard_text_watcher::clipboard_text_watcher(window& win, text_changed_callback changed)
    : _window(win)
    , _callback(std::move(changed))
{
#ifdef SDL_PLATFORM_WIN32
    watch_clipboard_changed(_window.handle(), _callback);
#endif
}

clipboard_text_watcher::~clipboard_text_watcher()
{
#ifdef SDL_PLATFORM_WIN32
    stop_watch_clipboard_changed(_window.handle());
#endif
}

} // namespace poc::platform

#else

#warning "clipboard_text_watcher is only supported on Windows platform"

namespace poc::platform {

clipboard_text_watcher::clipboard_text_watcher(window& win, text_changed_callback changed)
    : _window(win)
    , _callback(std::move(changed))
{
}

clipboard_text_watcher::~clipboard_text_watcher()
{
}

} // namespace poc::platform

#endif