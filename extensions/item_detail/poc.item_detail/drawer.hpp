#pragma once

#include <vector>

#include <poc.workspace/context.hpp>
#include <poc.workspace/drawer.hpp>
#include <poc.workspace/item_id.hpp>
#include <poc.workspace/selection.hpp>

namespace poc::item_detail {

class drawer
{
public:
    static constexpr auto id{workspace::make_drawer_id("poc.item_detail")};

    auto draw(workspace::draw_context& context) -> void;

    auto on_selection_changed(workspace::selected selected, std::span<const workspace::item_id> ids) -> void;

private:
    std::vector<workspace::item_id> _selected_items;
};

} // namespace poc::item_detail
