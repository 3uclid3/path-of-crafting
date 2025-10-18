#include <poc.workspace/context.hpp>

#include <poc.workspace/drawer_registry.hpp>

namespace poc::workspace {

init_context::init_context(drawer_registry& drawers)
    : _drawers(drawers)
{
}

auto init_context::add_drawer(drawer_id id, drawer draw, drawing_phase phase, draw_priority priority) -> bool
{
    return _drawers.add(id, draw, phase, priority);
}

update_context::update_context(drawer_registry& drawers)
    : _drawers(drawers)
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

} // namespace poc::workspace
