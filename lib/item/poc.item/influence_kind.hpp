#pragma once

#include <cstdint>
#include <format>

#include <poc.core/types.hpp>
#include <poc.core/utility/bitmask_enum.hpp>

namespace poc {

enum class influence_kind : u8
{
    none = 0x00,

    shaper = 1 << 0,
    elder = 1 << 1,

    warlord = 1 << 2,
    hunter = 1 << 3,
    redeemer = 1 << 4,
    crusader = 1 << 5,

    eater_of_worlds = 1 << 6,
    searing_exarch = 1 << 7,

    unknown = 0xFF
};

template<>
struct is_bitmask_enum<influence_kind> : std::true_type
{};

} // namespace poc

template<>
struct std::formatter<poc::influence_kind> : std::formatter<std::string_view>
{
    constexpr auto format(poc::influence_kind k, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(magic_enum::enum_name(k), ctx);
    }
};
