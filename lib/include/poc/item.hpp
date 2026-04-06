#pragma once

#include <string_view>

namespace poc {

struct item
{
    static auto is_blob(std::string_view text) noexcept -> bool;
    static auto is_magic_blob(std::string_view text) noexcept -> bool;

    static auto parse_name(std::string_view blob) noexcept -> std::string_view;
};

} // namespace poc
