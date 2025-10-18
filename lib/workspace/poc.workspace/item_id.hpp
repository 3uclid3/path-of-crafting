#pragma once

#include <type_traits>
#include <utility>

namespace poc::workspace {

enum class item_id : std::uint32_t
{
};

} // namespace poc::workspace

template<>
struct std::hash<poc::workspace::item_id>
{
    std::size_t operator()(poc::workspace::item_id id) const noexcept
    {
        using T = std::underlying_type_t<poc::workspace::item_id>;
        return std::hash<T>{}(static_cast<T>(id));
    }
};
