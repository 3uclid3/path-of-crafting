#include <poc.app/app.hpp>
#include <poc.gui.demo/extension.hpp>

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
    app.add_extension<gui::demo::extension>();
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
