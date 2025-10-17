#pragma once

#include <span>
#include <vector>

#include <poc.workspace/item_id.hpp>

namespace poc::workspace {

class selection
{
public:
    using storage_type = std::vector<item_id>;
    using value_type = storage_type::value_type;
    using size_type = storage_type::size_type;
    using iterator = storage_type::iterator;
    using const_iterator = storage_type::const_iterator;

public:
    [[nodiscard]] auto empty() const noexcept -> bool;
    [[nodiscard]] auto size() const noexcept -> size_type;

    [[nodiscard]] auto contains(value_type id) const noexcept -> bool;

    auto select(value_type value) -> void;
    auto deselect(value_type value) -> void;
    auto toggle(value_type value) -> void;

    auto select(std::span<const value_type> value) -> size_type;
    auto deselect(std::span<const value_type> value) -> size_type;

    auto clear() noexcept -> void;
    auto reserve(size_type size) -> void;

    [[nodiscard]] auto begin() const noexcept -> const_iterator;
    [[nodiscard]] auto end() const noexcept -> const_iterator;

    [[nodiscard]] auto begin() noexcept -> iterator;
    [[nodiscard]] auto end() noexcept -> iterator;

private:
    storage_type _items;
};

} // namespace poc::workspace
