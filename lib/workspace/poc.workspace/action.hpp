#pragma once

#include <poc.core/types.hpp>
#include <poc.core/utility/crc32.hpp>
#include <poc.core/utility/tagged.hpp>

namespace poc::workspace {

class action_context;

using action_id = tagged<u32, struct action_id_tag>;

consteval action_id make_action_id(string_view str) { return action_id{compile_crc32(str)}; }

enum class action_coalesce_policy : u8
{
    keep_all,
    replace_last,
    drop_if_exists
};

template<typename T>
struct action_traits
{
    using type = T;

    static constexpr action_id id{type::id};

    static constexpr action_coalesce_policy coalesce_policy{[]() -> action_coalesce_policy {
        if constexpr (requires { type::coalesce_policy; })
        {
            return type::coalesce_policy;
        }
        else
        {
            return action_coalesce_policy::keep_all;
        }
    }()};
};

} // namespace poc::workspace
