#include <poc/clipboard.hpp>

#define NOMINMAX
#include <windows.h>

namespace poc::clipboard {

namespace detail {

HWND g_hwnd = nullptr;
changed_callback g_changed_callback;

auto win_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT
{
    if (msg == WM_CLIPBOARDUPDATE)
    {
        if (OpenClipboard(nullptr))
        {
            if (HANDLE hData = GetClipboardData(CF_TEXT))
            {
                if (char* pszText = static_cast<char*>(GlobalLock(hData)))
                {
                    g_changed_callback(pszText);
                    GlobalUnlock(hData);
                }
                else
                {
                    g_changed_callback("");
                }
            }
            CloseClipboard();
        }
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

auto install_clipboard_watcher() -> void
{
    if (!detail::g_hwnd)
    {
        // Register a window class
        WNDCLASS wc{};
        wc.lpfnWndProc = &detail::win_proc;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = "PathOfCraftingClipboardWatcherWindowClass";
        RegisterClass(&wc);

        // Create a message-only window
        detail::g_hwnd = CreateWindowEx(0, wc.lpszClassName, nullptr, 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    }

    if (detail::g_hwnd)
    {
        // Register for clipboard update notifications
        AddClipboardFormatListener(detail::g_hwnd);
    }
}

auto uninstall_clipboard_watcher() -> void
{
    if (detail::g_hwnd)
    {
        // Unregister the clipboard format listener
        RemoveClipboardFormatListener(detail::g_hwnd);

        // Destroy the message-only window
        DestroyWindow(detail::g_hwnd);
        detail::g_hwnd = nullptr;
    }
}

} // namespace detail

auto set_changed_callback(changed_callback callback) -> void
{
    detail::g_changed_callback = callback;

    if (detail::g_changed_callback)
    {
        detail::install_clipboard_watcher();
    }
    else
    {
        detail::uninstall_clipboard_watcher();
    }
}

} // namespace poc::clipboard
