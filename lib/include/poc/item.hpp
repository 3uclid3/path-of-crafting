#pragma once

#include <string_view>

namespace poc {

struct item
{
    static auto is_magic_blob(std::string_view blob) noexcept -> bool;
    static auto parse_magic_name(std::string_view blob) noexcept -> std::string_view;
};

} // namespace poc
