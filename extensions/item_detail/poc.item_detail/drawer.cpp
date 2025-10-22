#include <poc.item_detail/drawer.hpp>

#include <poc.core/algorithm.hpp>
#include <poc.gui/widgets.hpp>
#include <poc.gui/window.hpp>
#include <poc.workspace/selection.hpp>

namespace poc::item_detail {

auto drawer::draw(workspace::draw_context& context) -> void
{
    if (gui::begin("Item Details"))
    {
        if (!_selected_items.empty())
        {
            const auto* item = context.items().get(_selected_items.front());

            gui::text("{}", *item);
        }
        else
        {
            gui::text("No item selected.");
        }
    }
    gui::end();
}

auto drawer::on_selection_changed(workspace::selected selected, std::span<const workspace::item_id> ids) -> void
{
    if (selected)
    {
        _selected_items.append_range(ids);
    }
    else
    {
        // keep them sorted
        for (const auto id : ids)
        {
            auto it = std::ranges::find(_selected_items, id);

            if (it != _selected_items.end())
            {
                _selected_items.erase(it);
            }
        }
    }
}

} // namespace poc::item_detail
