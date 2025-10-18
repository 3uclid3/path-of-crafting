#pragma once

#include <format>

#include <poc.core/types.hpp>

namespace poc {

// api without kind parameter is assumed to be tier
class modifier_tier
{
public:
    using value_type = u8;

    struct rank_tag
    {};

    constexpr modifier_tier() noexcept = default;
    constexpr modifier_tier(const modifier_tier&) noexcept = default;
    constexpr modifier_tier(modifier_tier&&) noexcept = default;

    constexpr modifier_tier(value_type value) noexcept;
    constexpr modifier_tier(value_type value, rank_tag) noexcept;

    constexpr auto operator=(const modifier_tier&) noexcept -> modifier_tier& = default;
    constexpr auto operator=(modifier_tier&&) noexcept -> modifier_tier& = default;

    constexpr auto operator<=>(const modifier_tier&) const noexcept -> std::strong_ordering;

    constexpr auto is_tier() const noexcept -> bool;
    constexpr auto is_rank() const noexcept -> bool;

    constexpr auto value() const noexcept -> value_type;

    constexpr auto set(value_type value) noexcept -> void;
    constexpr auto set_rank(value_type value) noexcept -> void;

    constexpr operator value_type() const noexcept;

private:
    static constexpr std::size_t kind_bits{1};
    static constexpr std::size_t value_bits{7};

    enum class kind : value_type
    {
        tier,
        rank
    };

    value_type _kind : kind_bits{std::to_underlying(kind::tier)};
    value_type _value : value_bits{0};
};

constexpr modifier_tier::modifier_tier(value_type value) noexcept
    : _value(value)
{
}

constexpr modifier_tier::modifier_tier(value_type value, rank_tag) noexcept
    : _kind{std::to_underlying(kind::rank)}
    , _value{value}
{
}

constexpr auto modifier_tier::operator<=>(const modifier_tier& other) const noexcept -> std::strong_ordering
{
    const auto kind_result = _kind <=> other._kind;

    return kind_result != std::strong_ordering::equal ? kind_result : _value <=> other._value;
}

constexpr auto modifier_tier::is_tier() const noexcept -> bool
{
    return _kind == std::to_underlying(kind::tier);
}

constexpr auto modifier_tier::is_rank() const noexcept -> bool
{
    return _kind == std::to_underlying(kind::rank);
}

constexpr auto modifier_tier::value() const noexcept -> value_type
{
    return _value;
}

constexpr auto modifier_tier::set(value_type value) noexcept -> void
{
    _kind = std::to_underlying(kind::tier);
    _value = value;
}

constexpr auto modifier_tier::set_rank(value_type value) noexcept -> void
{
    _kind = std::to_underlying(kind::rank);
    _value = value;
}

constexpr modifier_tier::operator value_type() const noexcept
{
    return value();
}

} // namespace poc

template<>
struct std::formatter<poc::modifier_tier> : std::formatter<poc::modifier_tier::value_type>
{
    auto format(poc::modifier_tier tier, std::format_context& ctx) const
    {
        return std::formatter<poc::modifier_tier::value_type>::format(tier.value(), ctx);
    }
};
