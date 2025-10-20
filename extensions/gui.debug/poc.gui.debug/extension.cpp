#include <poc.gui.debug/extension.hpp>

#include <poc.gui/debug.hpp>

namespace poc::gui::debug {

auto extension::id() const -> id_type
{
    return static_id;
}

auto extension::init(init_context& context) -> init_result
{
    if (!context.add_drawer(static_id, [](auto&) { draw_demo_window(); }))
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
