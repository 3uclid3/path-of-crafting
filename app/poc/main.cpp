#include <poc.core/log.hpp>
#include <poc.core/utility/on_scope_exit.hpp>
#include <poc.gui.backend.sdl3_opengl/init.hpp>
#include <poc.gui.backend.sdl3_opengl/render.hpp>
#include <poc.gui.core/demo.hpp>
#include <poc.gui.core/init.hpp>
#include <poc.gui.core/render.hpp>
#include <poc.platform/clipboard_text_watcher.hpp>
#include <poc.platform/window.hpp>

auto main() -> int
{
    using namespace poc;

    log::init();
    on_scope_exit log_uninit{log::uninit};

    platform::window window("Path of Crafting", 1280, 720);

    if (!window.is_open())
        return -1;

    gui::init();
    on_scope_exit gui_uninit{gui::uninit};

    gui::backend::sdl3_opengl::init(window);
    on_scope_exit gui_backend_uninit{gui::backend::sdl3_opengl::uninit};

    platform::clipboard_text_watcher watcher(window, [](std::string_view text) {
        POC_INFO("Clipboard text changed: {}", text);
    });

    while (window.is_open())
    {
        window.poll_events();

        gui::backend::sdl3_opengl::new_frame();

        gui::new_frame();
        gui::draw_demo();
        gui::render();

        window.clear();
        gui::backend::sdl3_opengl::render();
        window.present();
    }

    return 0;
}