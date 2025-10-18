#pragma once

#include <functional>
#include <optional>
#include <vector>

#include <poc.core/types.hpp>
#include <poc.platform/clipboard_text_watcher.hpp>
#include <poc.platform/window.hpp>
#include <poc.workspace/host.hpp>

namespace poc {

struct app_options
{
    string name;
};

class app final
{
public:
    explicit app(app_options options);

    app(const app&) = delete;
    auto operator=(const app&) -> app& = delete;

    app(app&&) = delete;
    auto operator=(app&&) -> app& = delete;

    ~app();

    template<typename T, typename... Args>
    requires std::derived_from<T, workspace::extension>
    auto add_extension(Args&&... args) -> workspace::extension*;

    auto init() -> bool;
    auto run() -> int;

private:
    using guard_type = std::function<void()>;

    auto update() -> void;

    app_options _options;

    std::optional<platform::window> _window;
    std::optional<platform::clipboard_text_watcher> _clipboard_watcher;

    workspace::host _host;

    std::vector<guard_type> _guards;
};

template<typename T, typename... Args>
requires std::derived_from<T, workspace::extension>
auto app::add_extension(Args&&... args) -> workspace::extension*
{
    return _host.add_extension<T>(std::forward<Args>(args)...);
}

} // namespace poc
