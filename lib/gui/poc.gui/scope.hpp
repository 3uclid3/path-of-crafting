#pragma once

#include <poc.core/types.hpp>

namespace poc::gui {

using id_t = int;

// push(hash)/pop ID stack
auto push_id(string_view str_id) -> void;
auto push_id(const void* ptr_id) -> void;
auto push_id(id_t id) -> void;
auto pop_id() -> void;

struct id_scope_guard
{
    id_scope_guard(string_view str_id);
    id_scope_guard(const void* ptr_id);
    id_scope_guard(id_t id);
    ~id_scope_guard();
};

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

inline id_scope_guard::id_scope_guard(id_t id)
{
    push_id(id);
}

inline id_scope_guard::~id_scope_guard()
{
    pop_id();
}

} // namespace poc::gui
