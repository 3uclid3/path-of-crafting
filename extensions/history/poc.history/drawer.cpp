#include <poc.history/drawer.hpp>

#include <poc.core/assert.hpp>
#include <poc.gui/scope.hpp>
#include <poc.gui/widgets.hpp>
#include <poc.gui/window.hpp>
#include <poc.workspace/context.hpp>
#include <poc.workspace/host.hpp>
#include <poc.workspace/item_repository.hpp>
#include <poc.workspace/selection.hpp>

namespace poc::history {

auto drawer::draw(workspace::draw_context& context) -> void
{
    gui::id_scope_guard id_guard{"poc.history.drawer"};

    const auto& items = context.items();

    if (gui::begin("Items"))
    {
        for (auto id : _items)
        {
            gui::id_scope_guard item_id_guard{static_cast<gui::id_t>(id)};

            const auto* item = items.get(id);
            POC_ASSERT(item != nullptr);

            const bool is_selected = context.selection().contains(id);
            if (gui::selectable(item->name(), is_selected))
            {
                if (is_selected)
                {
                    context.enqueue_action<workspace::deselect_item_action>(id);
                }
                else
                {
                    context.enqueue_action<workspace::select_item_action>(id);
                }
            }
        }
    }
    gui::end();
}

auto drawer::on_item_added(workspace::item_id item_id) -> void
{
    _items.push_back(item_id);
}

auto drawer::on_item_removed(workspace::item_id item_id) -> void
{
    auto it = std::find(_items.begin(), _items.end(), item_id);
    if (it != _items.end())
    {
        _items.erase(it);
    }
}

} // namespace poc::history
