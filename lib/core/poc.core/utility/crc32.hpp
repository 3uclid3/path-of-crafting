#pragma once

#include <poc.core/types.hpp>

namespace poc {

constexpr u32 crc32(string_view s) noexcept;
consteval u32 compile_crc32(string_view s) noexcept;

inline namespace literals {
constexpr u32 operator""_crc32(const char* s, std::size_t n) noexcept;
} // namespace literals

constexpr u32 crc32(string_view s) noexcept
{
    u32 crc = 0xFFFF'FFFFu;
    for (unsigned char b : s)
    {
        crc ^= b;
        for (int i = 0; i < 8; ++i)
        {
            const u32 mask = -(crc & 1u);
            crc = (crc >> 1) ^ (0xEDB88320u & mask);
        }
    }
    return ~crc;
}

consteval u32 compile_crc32(string_view s) noexcept
{
    return crc32(s);
}

inline namespace literals {
constexpr u32 operator""_crc32(const char* s, std::size_t n) noexcept
{
    return crc32(std::string_view{s, n});
}
} // namespace literals

} // namespace poc
