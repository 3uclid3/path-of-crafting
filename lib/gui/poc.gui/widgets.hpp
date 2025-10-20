#pragma once

#include <poc.core/types.hpp>

namespace poc::gui {

// ---------------------------------------------------------------
// ID

using id = u32;

struct id_scope_guard
{
    id_scope_guard(string_view str_id);
    id_scope_guard(const void* ptr_id);
    id_scope_guard(u32 id);
    ~id_scope_guard();
};

// push(hash)/pop ID stack
auto push_id(string_view str_id) -> void;
auto push_id(const void* ptr_id) -> void;
auto push_id(u32 id) -> void;
auto pop_id() -> void;

// ---------------------------------------------------------------
// Window

enum class window_flags
{
    none
};

auto begin(string_view name, window_flags flags = window_flags::none) -> bool;
auto begin(string_view name, bool& is_open, window_flags flags = window_flags::none) -> bool;
auto end() -> void;

// ---------------------------------------------------------------
// Widget: Text

template<typename... Args>
constexpr auto text(format_string<Args...> fmt, Args&&... args) -> void;
auto vtext(string_view fmt, format_args args) -> void;

// ---------------------------------------------------------------
// Widget: Selectable

enum class selectable_flags
{
    none
};

auto selectable(string_view label, bool is_selected, selectable_flags flags = selectable_flags::none) -> bool;

// ---------------------------------------------------------------
// Implementation

inline id_scope_guard::id_scope_guard(string_view str_id)
{
    push_id(str_id);
}

inline id_scope_guard::id_scope_guard(const void* ptr_id)
{
    push_id(ptr_id);
}

inline id_scope_guard::id_scope_guard(u32 id)
{
    push_id(id);
}

inline id_scope_guard::~id_scope_guard()
{
    pop_id();
}

template<typename... Args>
constexpr auto text(format_string<Args...> fmt, Args&&... args) -> void
{
    vtext(fmt.get(), std::make_format_args(args...));
}

} // namespace poc::gui
