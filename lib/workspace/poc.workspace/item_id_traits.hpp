#pragma once

#include <iostream>
#include <type_traits>

#include <poc.workspace/item_id.hpp>
#include <poc.workspace/log.hpp>

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
    auto version = static_cast<version_type>((to_version(value) + 1) % version_mask);
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

} // namespace poc::workspace
