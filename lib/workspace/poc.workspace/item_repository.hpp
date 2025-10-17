#pragma once

#include <chrono>
#include <cstdint>
#include <vector>

#include <poc.item/item.hpp>
#include <poc.workspace/item_id.hpp>
#include <poc.workspace/item_id_traits.hpp>

namespace poc::workspace {

class item_repository
{
public:
    using clock = std::chrono::system_clock;
    using time_point = clock::time_point;

    using value_type = item;
    using size_type = item_id_traits::index_type;

    using const_iterator = typename std::vector<value_type>::const_iterator;

public:
    explicit item_repository(size_type reserve = 4096);

    item_repository(const item_repository&) = delete;
    item_repository& operator=(const item_repository&) = delete;

    item_repository(item_repository&&) = delete;
    item_repository& operator=(item_repository&&) = delete;

    auto add(value_type&& item) -> item_id;
    auto remove(item_id id) -> bool;

    auto get(item_id id) const noexcept -> const item*;
    auto get_timestamp(item_id id) const noexcept -> time_point;

    auto begin() const noexcept -> const_iterator;
    auto end() const noexcept -> const_iterator;

    auto size() const noexcept -> size_type;
    auto empty() const noexcept -> bool;

private:
    using slot_type = item_id;
    using slot_traits = item_id_traits;

    static constexpr auto null_slot = null_item_id;
    static constexpr auto tombstone_slot = tombstone_item_id;

    std::vector<slot_type> _slots;
    std::vector<value_type> _dense;
    std::vector<size_type> _dense2slots;
    std::vector<time_point> _timestamps;

    size_type _free_head{slot_traits::to_index(null_slot)};
};

} // namespace poc::workspace
