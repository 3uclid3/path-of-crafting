#include <poc.workspace/item_repository.hpp>

namespace poc::workspace {

item_repository::item_repository(size_type reserve)
{
    _slots.reserve(reserve);
    _dense.reserve(reserve);
    _dense2slots.reserve(reserve);
    _timestamps.reserve(reserve);
}

auto item_repository::add(value_type&& item) -> item_id
{
    size_type slot_index;

    if (_free_head != invalid_slot_index)
    {
        auto& slot = _slots[_free_head];

        slot_index = std::exchange(_free_head, slot_traits::to_index(slot));
        slot = slot_traits::merge(_dense.size(), slot);
    }
    else
    {
        slot_index = static_cast<size_type>(_slots.size());
        _slots.push_back(slot_traits::construct(_dense.size(), 0));
    }

    _dense.push_back(std::move(item));
    _timestamps.push_back(clock::now());
    _dense2slots.push_back(slot_index);

    const item_id id = item_id_traits::merge(slot_index, _slots[slot_index]);
    _added.emit(id);

    return id;
}

auto item_repository::remove(item_id id) -> bool
{
    const size_type slot_index = item_id_traits::to_index(id);

    if (slot_index >= _slots.size())
    {
        return false;
    }

    auto& slot = _slots[slot_index];

    if (slot_traits::to_version(slot) != item_id_traits::to_version(id))
    {
        return false;
    }

    const size_type dense_index = slot_traits::to_index(slot);
    const size_type last_dense_index = _dense.size() - 1;
    const size_type last_slot_index = _dense2slots[last_dense_index];
    auto& last_slot = _slots[last_slot_index];

    if (dense_index != last_dense_index)
    {
        _dense[dense_index] = std::move(_dense[last_dense_index]);
        _timestamps[dense_index] = _timestamps[last_dense_index];
        _dense2slots[dense_index] = last_slot_index;
        last_slot = slot_traits::merge(dense_index, last_slot);
    }

    _dense.pop_back();
    _timestamps.pop_back();
    _dense2slots.pop_back();

    slot = slot_traits::merge(_free_head, slot_traits::next(slot));
    _free_head = slot_index;

    _removed.emit(id);

    return true;
}

auto item_repository::get(item_id id) const noexcept -> const item*
{
    const size_type slot_index = item_id_traits::to_index(id);

    if (slot_index >= _slots.size())
    {
        return nullptr;
    }

    const auto& slot = _slots[slot_index];

    if (slot_traits::to_version(slot) != item_id_traits::to_version(id))
    {
        return nullptr;
    }

    return &_dense[slot_traits::to_index(slot)];
}

auto item_repository::get_timestamp(item_id id) const noexcept -> time_point
{
    const size_type slot_index = item_id_traits::to_index(id);

    if (slot_index >= _slots.size())
    {
        return {};
    }

    const auto& slot = _slots[slot_index];

    if (slot_traits::to_version(slot) != item_id_traits::to_version(id))
    {
        return {};
    }

    return _timestamps[slot_traits::to_index(slot)];
}

auto item_repository::begin() const noexcept -> const_iterator
{
    return _dense.begin();
}

auto item_repository::end() const noexcept -> const_iterator
{
    return _dense.end();
}

auto item_repository::size() const noexcept -> size_type
{
    return static_cast<size_type>(_dense.size());
}

auto item_repository::empty() const noexcept -> bool
{
    return size() == 0;
}

auto item_repository::added() noexcept -> added_signal::connector_type
{
    return _added.connector();
}

auto item_repository::removed() noexcept -> removed_signal::connector_type
{
    return _removed.connector();
}

} // namespace poc::workspace
