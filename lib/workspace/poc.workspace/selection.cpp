#include <poc.workspace/selection.hpp>

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

auto selection::select(value_type value) -> void
{
    POC_ASSERT(!contains(value));

    _items.push_back(value);

    auto span = std::span<const value_type>{&value, 1};
    _changed.emit(selected(true), span);
}

auto selection::deselect(value_type value) -> void
{
    POC_ASSERT(contains(value));

    _items.erase(std::remove(_items.begin(), _items.end(), value), _items.end());

    auto span = std::span<const value_type>{&value, 1};
    _changed.emit(selected(false), span);
}

auto selection::toggle(value_type value) -> void
{
    if (contains(value))
    {
        deselect(value);
    }
    else
    {
        select(value);
    }
}

auto selection::select(std::span<const value_type> value) -> void
{
    POC_ASSERT(!std::ranges::any_of(value, [this](auto v) { return contains(v); }));

    _items.insert(_items.end(), value.begin(), value.end());

    _changed.emit(selected(true), value);
}

auto selection::deselect(std::span<const value_type> value) -> void
{
    POC_ASSERT(std::ranges::all_of(value, [this](auto v) { return contains(v); }));

    _items.erase(
        std::remove_if(_items.begin(), _items.end(), [value](auto v) {
            return std::ranges::contains(value, v);
        }),
        _items.end());

    _changed.emit(selected(false), value);
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

auto selection::begin() noexcept -> iterator
{
    return _items.begin();
}

auto selection::end() noexcept -> iterator
{
    return _items.end();
}

auto selection::changed() noexcept -> changed_signal&
{
    return _changed;
}

} // namespace poc::workspace
