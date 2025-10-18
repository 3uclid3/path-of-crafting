#pragma once

#include <string_view>
#include <type_traits>
#include <utility>

#define TEST_STRING(String) \
    [] -> std::basic_string_view<T> { \
        if constexpr (std::is_same_v<T, char>) \
            return String; \
        if constexpr (std::is_same_v<T, char8_t>) \
            return u8##String; \
        if constexpr (std::is_same_v<T, wchar_t>) \
            return L##String; \
        std::unreachable(); \
    }()
