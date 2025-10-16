#pragma once

#include <poc/container/static_vector.hpp>
#include <poc/modifier_tier.hpp>
#include <poc/types.hpp>

namespace poc {

class modifier
{
public:
    using tier_type = modifier_tier;

    constexpr modifier() noexcept = default;
    constexpr modifier(const modifier&) noexcept = default;
    constexpr modifier(modifier&&) noexcept = default;

    constexpr modifier(string_view name, string_view lines, string_view tags, tier_type tier) noexcept;

    constexpr auto operator=(const modifier&) noexcept -> modifier& = default;
    constexpr auto operator=(modifier&&) noexcept -> modifier& = default;

    constexpr auto operator==(const modifier&) const noexcept -> bool = default;

    constexpr auto name() const noexcept -> string_view;
    constexpr auto tier() const noexcept -> tier_type;
    constexpr auto tags() const noexcept -> string_view;
    constexpr auto lines() const noexcept -> string_view;

private:
    string_view _name;
    string_view _lines;
    string_view _tags;
    tier_type _tier;
};

constexpr modifier::modifier(string_view name, string_view lines, string_view tags, tier_type tier) noexcept
    : _name(name)
    , _lines(lines)
    , _tags(tags)
    , _tier(tier)
{
}

constexpr auto modifier::name() const noexcept -> string_view
{
    return _name;
}

constexpr auto modifier::tier() const noexcept -> tier_type
{
    return _tier;
}

constexpr auto modifier::tags() const noexcept -> string_view
{
    return _tags;
}

constexpr auto modifier::lines() const noexcept -> string_view
{
    return _lines;
}

} // namespace poc
