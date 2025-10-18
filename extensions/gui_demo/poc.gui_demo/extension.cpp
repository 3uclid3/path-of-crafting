#include <poc.gui_demo/extension.hpp>

#include <poc.gui.core/demo.hpp>

namespace poc::gui_demo {

auto extension::id() const -> id_type
{
    return static_id;
}

auto extension::init(init_context& context) -> init_result
{
    if (!context.add_drawer("poc.gui.demo", [](auto&) { gui::draw_demo(); }))
    {
        return init_result::failure;
    }

    return init_result::success;
}

auto extension::update(update_context&) -> void
{
    // nothing to do
}

} // namespace poc::gui_demo
