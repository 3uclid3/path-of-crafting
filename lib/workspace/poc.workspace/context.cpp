#include <poc.workspace/context.hpp>

namespace poc::workspace {

init_context::init_context(items_type& items, selection_type& selection, drawers_type& drawers)
    : _items(items)
    , _selection(selection)
    , _drawers(drawers)
{
}

auto init_context::add_drawer(drawer_id id, drawer draw, drawing_phase phase, draw_priority priority) -> bool
{
    return _drawers.add(id, draw, phase, priority);
}

auto init_context::item_added() -> items_type::added_signal::connector_type
{
    return _items.added();
}

auto init_context::item_removed() -> items_type::removed_signal::connector_type
{
    return _items.removed();
}

auto init_context::selection_changed() -> selection_type::changed_signal::connector_type
{
    return _selection.changed();
}

update_context::update_context(const items_type& items, drawers_type& drawers)
    : _items(items)
    , _drawers(drawers)
{
}

auto update_context::add_drawer(drawer_id id, drawer draw, drawing_phase phase, draw_priority priority) -> bool
{
    return _drawers.add(id, draw, phase, priority);
}

auto update_context::remove_drawer(drawer_id id) -> bool
{
    return _drawers.remove(id);
}

auto update_context::items() const -> const items_type&
{
    return _items;
}

draw_context::draw_context(const items_type& items, const selection_type& selection, action_dispatcher_type& action_dispatcher)
    : _items(items)
    , _selection(selection)
    , _action_dispatcher(action_dispatcher)
{
}

auto draw_context::items() const -> const items_type&
{
    return _items;
}

auto draw_context::selection() const -> const selection_type&
{
    return _selection;
}

} // namespace poc::workspace
