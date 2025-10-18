#include <poc.app/app.hpp>

#include <poc.core/log.hpp>
#include <poc.gui.backend.sdl3_opengl/init.hpp>
#include <poc.gui.backend.sdl3_opengl/render.hpp>
#include <poc.gui.core/draw.hpp>
#include <poc.gui.core/init.hpp>

namespace poc {

namespace {

auto fail(string_view where) -> bool
{
    POC_ERROR("Failed to initialize {}", where);
    return false;
}

} // namespace

app::app(app_options options)
    : _options{std::move(options)}
{
}

app::~app()
{
    while (!_guards.empty())
    {
        _guards.back()();
        _guards.pop_back();
    }
}

auto app::init() -> bool
{
    // log
    log::init();
    _guards.emplace_back(log::uninit);

    // window
    _window.emplace(_options.name, 1280, 720);
    if (!_window->is_open())
        return fail("window");
    _guards.emplace_back([this]() { _window.reset(); });

    // imgui
    gui::init();
    _guards.emplace_back(gui::uninit);

    gui::backend::sdl3_opengl::init(*_window);
    _guards.emplace_back(gui::backend::sdl3_opengl::uninit);

    // workspace
    if (!_host.init())
        return fail("workspace host");
    _guards.emplace_back([this]() { _host.uninit(); });

    // clipboard watcher
    _clipboard_watcher.emplace(*_window, [this](string_view text) {
        if (_host.ingester().can_submit(text))
        {
            _host.ingester().submit(item_blob{text});
        }
    });
    _guards.emplace_back([this]() { _clipboard_watcher.reset(); });

    return true;
}

auto app::run() -> int
{
    while (_window->is_open())
    {
        update();
    }

    return 0;
}

auto app::update() -> void
{
    // platform and workspace updating
    _window->poll_events();
    _host.update();

    // gui building
    gui::backend::sdl3_opengl::new_frame();
    gui::new_frame();
    _host.draw();
    gui::end_frame();

    // rendering
    _window->clear();
    gui::backend::sdl3_opengl::render();
    _window->present();
}

} // namespace poc
