#pragma once

#include <type_traits>

#include <poc.workspace/item_id.hpp>

namespace poc::workspace {

struct item_id_traits
{
    using value_type = item_id;

    using integral_type = std::uint32_t;
    using index_type = std::uint32_t;
    using version_type = std::uint16_t;

    static constexpr integral_type index_mask{0xFFFFF};
    static constexpr integral_type version_mask{0xFFF};

    [[nodiscard]] static constexpr auto to_integral(value_type value) noexcept -> integral_type;
    [[nodiscard]] static constexpr auto to_index(value_type value) noexcept -> index_type;
    [[nodiscard]] static constexpr auto to_version(value_type value) noexcept -> version_type;

    [[nodiscard]] static constexpr auto construct(index_type value, version_type version) noexcept -> value_type;
    [[nodiscard]] static constexpr auto next(value_type value) noexcept -> value_type;

    [[nodiscard]] static constexpr auto merge(auto value_for_index, auto value_for_version) noexcept -> value_type;

    static_assert(sizeof(value_type) == sizeof(integral_type), "invalid size");
    static_assert(index_mask && ((index_mask & (index_mask + 1)) == 0), "invalid index mask");
    static_assert((version_mask & (version_mask + 1)) == 0, "invalid version mask");
};

struct null_item_id_t
{
    [[nodiscard]] constexpr operator item_id() const noexcept;

    [[nodiscard]] constexpr auto operator==(null_item_id_t) const noexcept -> bool;
    [[nodiscard]] constexpr auto operator!=(null_item_id_t) const noexcept -> bool;
    [[nodiscard]] constexpr auto operator==(item_id item_id) const noexcept -> bool;
    [[nodiscard]] constexpr auto operator!=(item_id item_id) const noexcept -> bool;
};

[[nodiscard]] constexpr auto operator==(item_id lhs, null_item_id_t rhs) noexcept -> bool;

static inline constexpr null_item_id_t null_item_id{};

struct tombstone_item_id_t
{
    [[nodiscard]] constexpr operator item_id() const noexcept;

    [[nodiscard]] constexpr auto operator==(tombstone_item_id_t) const noexcept -> bool;
    [[nodiscard]] constexpr auto operator!=(tombstone_item_id_t) const noexcept -> bool;
    [[nodiscard]] constexpr auto operator==(item_id item_id) const noexcept -> bool;
    [[nodiscard]] constexpr auto operator!=(item_id item_id) const noexcept -> bool;
};

[[nodiscard]] constexpr auto operator==(item_id lhs, tombstone_item_id_t rhs) noexcept -> bool;

static inline constexpr tombstone_item_id_t tombstone_item_id{};

constexpr auto item_id_traits::to_integral(value_type value) noexcept -> integral_type
{
    return static_cast<integral_type>(value);
}

constexpr auto item_id_traits::to_index(value_type value) noexcept -> index_type
{
    return to_integral(value) & index_mask;
}

constexpr auto item_id_traits::to_version(value_type value) noexcept -> version_type
{
    return static_cast<version_type>((to_integral(value) >> std::popcount(index_mask)) & version_mask);
}

constexpr auto item_id_traits::construct(index_type value, version_type version) noexcept -> value_type
{
    return static_cast<value_type>((value & index_mask) | ((version & version_mask) << std::popcount(index_mask)));
}

constexpr auto item_id_traits::next(value_type value) noexcept -> value_type
{
    const index_type index = to_index(value);
    auto version = static_cast<version_type>((to_version(value) + 1) & version_mask);
    if (version == version_mask)
    {
        version = 0;
    }
    return construct(index, version);
}

constexpr auto item_id_traits::merge(auto value_for_index, auto value_for_version) noexcept -> value_type
{
    using index_t = std::decay_t<decltype(value_for_index)>;
    using version_t = std::decay_t<decltype(value_for_version)>;

    if constexpr (!std::is_same_v<index_t, integral_type>)
    {
        return merge(to_integral(static_cast<value_type>(value_for_index)), value_for_version);
    }
    else if constexpr (!std::is_same_v<version_t, integral_type>)
    {
        return merge(value_for_index, to_integral(static_cast<value_type>(value_for_version)));
    }
    else
    {
        static_assert(std::is_same_v<index_t, integral_type>, "invalid value type");
        static_assert(std::is_same_v<version_t, integral_type>, "invalid version type");

        return static_cast<value_type>((value_for_index & index_mask) | (value_for_version & (version_mask << std::popcount(index_mask))));
    }
}

constexpr null_item_id_t::operator item_id() const noexcept
{
    return item_id_traits::construct(item_id_traits::index_mask, item_id_traits::version_mask);
}

constexpr auto null_item_id_t::operator==(null_item_id_t) const noexcept -> bool
{
    return true;
}

constexpr auto null_item_id_t::operator!=(null_item_id_t) const noexcept -> bool
{
    return false;
}

constexpr auto null_item_id_t::operator==(item_id id) const noexcept -> bool
{
    return item_id_traits::to_index(id) == item_id_traits::to_index(*this);
}

constexpr auto null_item_id_t::operator!=(item_id id) const noexcept -> bool
{
    return !operator==(id);
}

constexpr auto operator==(item_id lhs, null_item_id_t rhs) noexcept -> bool
{
    return rhs.operator==(lhs);
}

constexpr tombstone_item_id_t::operator item_id() const noexcept
{
    return item_id_traits::construct(item_id_traits::index_mask, item_id_traits::version_mask);
}

constexpr auto tombstone_item_id_t::operator==(tombstone_item_id_t) const noexcept -> bool
{
    return true;
}

constexpr auto tombstone_item_id_t::operator!=(tombstone_item_id_t) const noexcept -> bool
{
    return false;
}

constexpr auto tombstone_item_id_t::operator==(item_id id) const noexcept -> bool
{
    return item_id_traits::to_version(id) == item_id_traits::to_version(*this);
}

constexpr auto tombstone_item_id_t::operator!=(item_id id) const noexcept -> bool
{
    return !operator==(id);
}

constexpr auto operator==(item_id lhs, tombstone_item_id_t rhs) noexcept -> bool
{
    return rhs.operator==(lhs);
}

} // namespace poc::workspace
