#pragma once

#include <poc.workspace/action_dispatcher.hpp>
#include <poc.workspace/drawer.hpp>
#include <poc.workspace/drawer_registry.hpp>
#include <poc.workspace/item_repository.hpp>
#include <poc.workspace/selection.hpp>

namespace poc::workspace {

class init_context
{
public:
    using items_type = item_repository;
    using selection_type = selection;
    using drawers_type = drawer_registry;

    init_context(items_type& items, selection_type& selection, drawers_type& drawers);

    template<typename T>
    auto add_drawer(T* obj, auto (T::*mf)(draw_context&), drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;

    template<typename T>
    auto add_drawer(const T* obj, auto (T::*mf)(draw_context&) const, drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;

    template<typename T>
    auto add_drawer(T& obj, auto (T::*mf)(draw_context&), drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;

    template<typename T>
    auto add_drawer(const T& obj, auto (T::*mf)(draw_context&) const, drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;

    auto add_drawer(drawer_id id, drawer draw, drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;

    [[nodiscard]] auto item_added() -> items_type::added_signal::connector_type;
    [[nodiscard]] auto item_removed() -> items_type::removed_signal::connector_type;

    [[nodiscard]] auto selection_changed() -> selection_type::changed_signal::connector_type;

private:
    items_type& _items;
    selection_type& _selection;
    drawers_type& _drawers;
};

class update_context
{
public:
    using items_type = item_repository;
    using drawers_type = drawer_registry;

    update_context(const items_type& items, drawers_type& drawers);

    template<typename T>
    auto add_drawer(T* obj, auto (T::*mf)(draw_context&), drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;

    template<typename T>
    auto add_drawer(const T* obj, auto (T::*mf)(draw_context&) const, drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;

    template<typename T>
    auto add_drawer(T& obj, auto (T::*mf)(draw_context&), drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;

    template<typename T>
    auto add_drawer(const T& obj, auto (T::*mf)(draw_context&) const, drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;

    auto add_drawer(drawer_id id, drawer draw, drawing_phase phase = drawing_phase::main, draw_priority priority = draw_priority{0}) -> bool;
    auto remove_drawer(drawer_id id) -> bool;

    auto items() const -> const items_type&;

private:
    const items_type& _items;
    drawers_type& _drawers;
};

class draw_context
{
public:
    using items_type = item_repository;
    using selection_type = selection;
    using action_dispatcher_type = action_dispatcher;

    draw_context(const items_type& items, const selection_type& selection, action_dispatcher_type& action_dispatcher);

    template<typename Action, typename... Args>
    auto enqueue_action(Args&&... args) -> void;

    auto items() const -> const items_type&;
    auto selection() const -> const selection_type&;

private:
    const items_type& _items;
    const selection_type& _selection;
    action_dispatcher_type& _action_dispatcher;
};

class action_context
{
public:
};

template<typename T>
auto init_context::add_drawer(T* obj, auto (T::*mf)(draw_context&), drawing_phase phase, draw_priority priority) -> bool
{
    POC_ASSERT(obj != nullptr);
    return add_drawer(*obj, mf, phase, priority);
}

template<typename T>
auto init_context::add_drawer(const T* obj, auto (T::*mf)(draw_context&) const, drawing_phase phase, draw_priority priority) -> bool
{
    POC_ASSERT(obj != nullptr);
    return add_drawer(*obj, mf, phase, priority);
}

template<typename T>
auto init_context::add_drawer(T& obj, auto (T::*mf)(draw_context&), drawing_phase phase, draw_priority priority) -> bool
{
    return add_drawer(T::id, [&obj, mf](draw_context& context) { std::invoke(mf, obj, context); }, phase, priority);
}

template<typename T>
auto init_context::add_drawer(const T& obj, auto (T::*mf)(draw_context&) const, drawing_phase phase, draw_priority priority) -> bool
{
    return add_drawer(T::id, [&obj, mf](draw_context& context) { std::invoke(mf, obj, context); }, phase, priority);
}

template<typename T>
auto update_context::add_drawer(T* obj, auto (T::*mf)(draw_context&), drawing_phase phase, draw_priority priority) -> bool
{
    POC_ASSERT(obj != nullptr);
    return add_drawer(*obj, mf, phase, priority);
}

template<typename T>
auto update_context::add_drawer(const T* obj, auto (T::*mf)(draw_context&) const, drawing_phase phase, draw_priority priority) -> bool
{
    POC_ASSERT(obj != nullptr);
    return add_drawer(*obj, mf, phase, priority);
}

template<typename T>
auto update_context::add_drawer(T& obj, auto (T::*mf)(draw_context&), drawing_phase phase, draw_priority priority) -> bool
{
    return add_drawer(T::id, [&obj, mf](draw_context& context) { std::invoke(mf, obj, context); }, phase, priority);
}

template<typename T>
auto update_context::add_drawer(const T& obj, auto (T::*mf)(draw_context&) const, drawing_phase phase, draw_priority priority) -> bool
{
    return add_drawer(T::id, [&obj, mf](draw_context& context) { std::invoke(mf, obj, context); }, phase, priority);
}

template<typename Action, typename... Args>
auto draw_context::enqueue_action(Args&&... args) -> void
{
    _action_dispatcher.enqueue<Action>(std::forward<Args>(args)...);
}

} // namespace poc::workspace
