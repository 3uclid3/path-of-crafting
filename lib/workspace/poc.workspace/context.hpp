#pragma once

#include <poc.workspace/drawer.hpp>

namespace poc::workspace {

class host;
class drawer_registry;

class init_context
{
public:
    explicit init_context(drawer_registry& drawers);

    auto add_drawer(drawer_id id, drawer draw, drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;

private:
    drawer_registry& _drawers;
};

class update_context
{
public:
    explicit update_context(drawer_registry& drawers);

    auto add_drawer(drawer_id id, drawer draw, drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;
    auto remove_drawer(drawer_id id) -> bool;

private:
    drawer_registry& _drawers;
};

class draw_context
{
public:
};

class action_context
{
public:
};

} // namespace poc::workspace
