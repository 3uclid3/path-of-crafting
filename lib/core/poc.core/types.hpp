#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

#include <magic_enum/magic_enum.hpp>

#include <poc.core/string/string_arena.hpp>
#include <poc.core/string/string_slice.hpp>

namespace poc {

// Default page size for string arenas: 256 KiB
static inline constexpr std::size_t default_string_arena_page_size{1024 * 256};

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;

using string = std::string;
using string_view = std::string_view;
using string_slice = basic_string_slice<char>;
using string_arena = basic_string_arena<char, default_string_arena_page_size>;
using string_lease = string_arena::lease;

extern template class basic_string_arena<char, default_string_arena_page_size>;
extern template class basic_string_slice<char>;

} // namespace poc
