#pragma once

#include <vector>

#include <poc.workspace/context.hpp>
#include <poc.workspace/drawer.hpp>

namespace poc::history {

class drawer
{
public:
    static constexpr auto id{workspace::make_drawer_id("poc.history")};

    auto draw(workspace::draw_context& context) -> void;

    auto on_item_added(workspace::item_id item) -> void;
    auto on_item_removed(workspace::item_id item) -> void;

private:
    std::vector<workspace::item_id> _items;
};

} // namespace poc::history
