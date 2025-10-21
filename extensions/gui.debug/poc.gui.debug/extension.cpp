#include <poc.gui.debug/extension.hpp>

#include <poc.gui/debug.hpp>
#include <poc.workspace/drawer.hpp>

namespace poc::gui::debug {

auto extension::id() const -> id_type
{
    return static_id;
}

auto extension::init(init_context& context) -> init_result
{
    if (!context.add_drawer(workspace::make_drawer_id("demo"), [](auto&) { draw_demo_window(); }))
    {
        return init_result::failure;
    }

    return init_result::success;
}

auto extension::update(update_context&) -> void
{
    // nothing to do
}

} // namespace poc::gui::debug
