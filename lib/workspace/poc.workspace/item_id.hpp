#pragma once

#include <poc.core/types.hpp>

namespace poc::workspace {

enum class item_id : u32
{
};

} // namespace poc::workspace

template<>
struct std::formatter<poc::workspace::item_id> : std::formatter<std::underlying_type_t<poc::workspace::item_id>>
{
    constexpr auto format(poc::workspace::item_id id, std::format_context& ctx) const
    {
        return std::formatter<std::underlying_type_t<poc::workspace::item_id>>::format(std::to_underlying(id), ctx);
    }
};
