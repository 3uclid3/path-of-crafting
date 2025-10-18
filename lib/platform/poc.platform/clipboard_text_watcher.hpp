#pragma once

#include <functional>

#include <poc.core/types.hpp>

namespace poc::platform {

class window;

class clipboard_text_watcher
{
public:
    using text_changed_callback = std::function<void(string_view)>;

    clipboard_text_watcher(window& win, text_changed_callback changed);
    ~clipboard_text_watcher();

    clipboard_text_watcher(const clipboard_text_watcher&) = delete;
    auto operator=(const clipboard_text_watcher&) -> clipboard_text_watcher& = delete;

    clipboard_text_watcher(clipboard_text_watcher&&) = delete;
    auto operator=(clipboard_text_watcher&&) -> clipboard_text_watcher& = delete;

private:
    window& _window;
    text_changed_callback _callback;
};

} // namespace poc::platform
