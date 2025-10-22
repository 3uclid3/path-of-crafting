#include <poc.app/app.hpp>

// Extensions
#include <poc.history/extension.hpp>
#include <poc.item_detail/extension.hpp>

// Debug Extensions
#include <poc.gui.debug/extension.hpp>

namespace poc {

auto create_options() -> app_options
{
    app_options options{
        .name = "Path of Crafting",
    };
    return options;
}

auto add_extensions(app& app) -> void
{
    app.add_extension<history::extension>();
    app.add_extension<item_detail::extension>();

    // Debug
    app.add_extension<gui::debug::extension>();
}

} // namespace poc

auto main() -> int
{
    poc::app app{poc::create_options()};
    poc::add_extensions(app);

    if (!app.init())
    {
        return -1;
    }

    return app.run();
}
