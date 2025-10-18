#pragma once

#include <unordered_map>
#include <vector>

#include <poc.core/types.hpp>
#include <poc.workspace/drawer.hpp>

namespace poc::workspace {

class draw_context;

class drawer_registry
{
public:
    auto add(drawer_id id, drawer draw, drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;
    auto remove(drawer_id id) -> bool;

    auto draw_all(draw_context& ctx) -> void;

private:
    struct drawer_meta
    {
        drawer_id id;
        drawing_phase phase;
        draw_priority priority;
    };

    using metas = std::vector<drawer_meta>;
    using drawers = std::vector<drawer>;
    using indexes = std::unordered_map<drawer_id, std::size_t>;

    // sorted
    drawing_phases<metas> _metas{};
    drawing_phases<drawers> _drawers{};

    // id to index
    drawing_phases<indexes> _indexes{};
};

} // namespace poc::workspace
