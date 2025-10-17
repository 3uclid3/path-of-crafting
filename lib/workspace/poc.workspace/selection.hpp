#pragma once

#include <span>
#include <vector>

#include <poc.core/signal.hpp>
#include <poc.core/utility/tagged.hpp>
#include <poc.workspace/item_id.hpp>

namespace poc::workspace {

using selected = tagged<bool, struct selected_tag>;

class selection
{
public:
    using storage_type = std::vector<item_id>;
    using value_type = storage_type::value_type;
    using size_type = storage_type::size_type;
    using const_iterator = storage_type::const_iterator;

public:
    using changed_signal = inplace_signal<8, selected, std::span<const value_type>>;

public:
    [[nodiscard]] auto empty() const noexcept -> bool;
    [[nodiscard]] auto size() const noexcept -> size_type;

    [[nodiscard]] auto contains(value_type id) const noexcept -> bool;

    auto select(value_type value) -> bool;
    auto deselect(value_type value) -> bool;
    auto toggle(value_type value) -> selected;

    auto select(std::span<const value_type> value) -> size_type;
    auto deselect(std::span<const value_type> value) -> size_type;

    auto clear() noexcept -> void;
    auto reserve(size_type size) -> void;

    [[nodiscard]] auto begin() const noexcept -> const_iterator;
    [[nodiscard]] auto end() const noexcept -> const_iterator;

    [[nodiscard]] auto changed() noexcept -> changed_signal&;

private:
    auto select_no_check(value_type value) -> void;
    auto deselect_no_check(value_type value, storage_type::iterator it) -> void;

    storage_type _items;
    changed_signal _changed;
};

} // namespace poc::workspace
