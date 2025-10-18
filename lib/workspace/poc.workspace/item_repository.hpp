#pragma once

#include <chrono>
#include <cstdint>
#include <span>
#include <vector>

#include <poc.core/signal.hpp>
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
    using added_signal = inplace_signal<8, item_id>;
    using removed_signal = inplace_signal<8, item_id>;

public:
    explicit item_repository(size_type reserve = 4096);

    item_repository(const item_repository&) = delete;
    item_repository& operator=(const item_repository&) = delete;

    item_repository(item_repository&&) = delete;
    item_repository& operator=(item_repository&&) = delete;

    auto add(value_type&& item) -> item_id;
    auto remove(item_id id) -> bool;
    auto remove(std::span<const item_id> ids) -> size_type;

    [[nodiscard]] auto get(item_id id) const noexcept -> const item*;
    [[nodiscard]] auto get_timestamp(item_id id) const noexcept -> time_point;

    [[nodiscard]] auto begin() const noexcept -> const_iterator;
    [[nodiscard]] auto end() const noexcept -> const_iterator;

    [[nodiscard]] auto size() const noexcept -> size_type;
    [[nodiscard]] auto empty() const noexcept -> bool;

    [[nodiscard]] auto added() noexcept -> added_signal&;
    [[nodiscard]] auto removed() noexcept -> removed_signal&;

private:
    using slot_type = item_id;
    using slot_traits = item_id_traits;

    static constexpr size_type invalid_slot_index{slot_traits::index_mask + 1};

    std::vector<slot_type> _slots;
    std::vector<value_type> _dense;
    std::vector<size_type> _dense2slots;
    std::vector<time_point> _timestamps;

    size_type _free_head{invalid_slot_index};

    added_signal _added;
    removed_signal _removed;
};

} // namespace poc::workspace
