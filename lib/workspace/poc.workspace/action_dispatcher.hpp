#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

#include <poc.core/algorithm.hpp>
#include <poc.core/assert.hpp>
#include <poc.core/types.hpp>
#include <poc.core/utility/always_false.hpp>
#include <poc.workspace/action.hpp>

namespace poc::workspace {

class action_dispatcher
{
public:
    template<typename Action, typename F>
    auto register_handler(F&& handler) -> bool;

    template<typename Action, typename... Args>
    auto enqueue(Args&&... args) -> void;

    auto dispatch(action_context& context) -> void;

private:
    // Binder receives a pointer to a T (the typed action), and must
    // return a closure that MOVES the action data immediately.
    using action_ptr = void*;
    using action_executor = std::move_only_function<void(action_context&)>;
    using action_binder = std::move_only_function<action_executor(action_ptr)>;

    struct action_entry
    {
        action_executor executor;
        action_id id;
    };

    std::unordered_map<action_id, action_binder> _binders;

    std::vector<action_entry> _in;
    std::vector<action_entry> _out;
};

template<typename Action, typename F>
auto action_dispatcher::register_handler(F&& handler) -> bool
{
    using traits = action_traits<Action>;

    if (_binders.contains(traits::id))
    {
        return false;
    }

    _binders[traits::id] = [handler = std::forward<F>(handler)](action_ptr ptr) {
        Action& data = *static_cast<Action*>(ptr);
        return [data = std::move(data), &handler](action_context& context) {
            handler(context, data);
        };
    };

    return true;
}

template<typename Action, typename... Args>
auto action_dispatcher::enqueue(Args&&... args) -> void
{
    using traits = action_traits<Action>;

    auto create_executor = [&]() {
        Action data{std::forward<Args>(args)...};

        auto it = _binders.find(traits::id);
        POC_ASSERT(it != _binders.end());

        action_binder& binder = it->second;
        return binder(&data);
    };

    if constexpr (traits::coalesce_policy == action_coalesce_policy::keep_all)
    {
        _in.emplace_back(create_executor(), traits::id);
    }
    else if constexpr (traits::coalesce_policy == action_coalesce_policy::replace_last)
    {
        if (!_in.empty() && _in.back().id == traits::id)
        {
            _in.back().executor = create_executor();
        }
        else
        {
            _in.emplace_back(create_executor(), traits::id);
        }
    }
    else if constexpr (traits::coalesce_policy == action_coalesce_policy::drop_if_exists)
    {
        if (std::ranges::none_of(_in, [&](const auto& entry) { return entry.id == traits::id; }))
        {
            _in.emplace_back(create_executor(), traits::id);
        }
    }
    else
    {
        static_assert(always_false_v<Action>, "Unhandled action coalesce policy");
    }
}

} // namespace poc::workspace
