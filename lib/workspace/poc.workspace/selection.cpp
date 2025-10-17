#include <poc.workspace/selection.hpp>

#include <array>
#include <memory_resource>

#include <poc.core/algorithm.hpp>
#include <poc.core/assert.hpp>

namespace poc::workspace {

auto selection::empty() const noexcept -> bool
{
    return _items.empty();
}

auto selection::size() const noexcept -> size_type
{
    return _items.size();
}

auto selection::contains(value_type id) const noexcept -> bool
{
    return std::ranges::find(_items, id) != _items.end();
}

auto selection::select(value_type value) -> bool
{
    if (contains(value))
    {
        return false;
    }

    add_and_emit(value);
    return true;
}

auto selection::deselect(value_type value) -> bool
{
    auto it = std::ranges::find(_items, value);
    if (it == _items.end())
    {
        return false;
    }

    remove_and_emit(value, it);
    return true;
}

auto selection::toggle(value_type value) -> selected
{
    auto it = std::ranges::find(_items, value);

    if (it == _items.end())
    {
        add_and_emit(value);
        return selected(true);
    }
    else
    {
        remove_and_emit(value, it);
        return selected(false);
    }
}

auto selection::select(std::span<const value_type> value) -> size_type
{
    std::array<std::byte, 1024> buffer;
    std::pmr::monotonic_buffer_resource resource{buffer.data(), buffer.size()};
    std::pmr::vector<value_type> newly_selected{&resource};

    for (auto v : value)
    {
        if (!contains(v))
        {
            newly_selected.push_back(v);
        }
    }

    if (!newly_selected.empty())
    {
        _items.append_range(newly_selected);
        _changed.emit(selected(true), std::span<const value_type>{newly_selected.data(), newly_selected.size()});
    }

    return newly_selected.size();
}

auto selection::deselect(std::span<const value_type> value) -> size_type
{
    std::array<std::byte, 1024> buffer;
    std::pmr::monotonic_buffer_resource resource{buffer.data(), buffer.size()};
    std::pmr::vector<value_type> newly_deselected{&resource};

    auto end = _items.end();

    for (auto v : value)
    {
        auto it = std::find(_items.begin(), end, v);

        if (it == end)
        {
            continue;
        }

        auto last = end - 1;

        if (it != last)
        {
            std::iter_swap(it, last);
        }

        end = last;
        newly_deselected.push_back(v);
    }

    if (!newly_deselected.empty())
    {
        _items.resize(_items.size() - newly_deselected.size());
        _changed.emit(selected(false), std::span<const value_type>{newly_deselected.data(), newly_deselected.size()});
    }

    return newly_deselected.size();
}

auto selection::clear() noexcept -> void
{
    _items.clear();
}

auto selection::reserve(size_type size) -> void
{
    _items.reserve(size);
}

auto selection::begin() const noexcept -> const_iterator
{
    return _items.begin();
}

auto selection::end() const noexcept -> const_iterator
{
    return _items.end();
}

auto selection::changed() noexcept -> changed_signal&
{
    return _changed;
}

auto selection::add_and_emit(value_type value) -> void
{
    _items.push_back(value);
    _changed.emit(selected(true), std::span<const value_type>{&value, 1});
}

auto selection::remove_and_emit(value_type value, storage_type::iterator it) -> void
{
    auto last = _items.end() - 1;
    if (it != last)
    {
        std::iter_swap(it, last);
        it = last; // now points to the value to be removed
    }

    _items.pop_back();
    _changed.emit(selected(false), std::span<const value_type>{&value, 1});
}

} // namespace poc::workspace
