#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <poc.core/assert.hpp>
#include <poc.core/types.hpp>
#include <poc.item/item.hpp>
#include <poc.workspace/action.hpp>
#include <poc.workspace/drawer_registry.hpp>
#include <poc.workspace/extension.hpp>
#include <poc.workspace/ingester.hpp>
#include <poc.workspace/selection.hpp>
#include <poc.workspace/store.hpp>

namespace poc::workspace {

class host
{
public:
    using action_dispatcher_type = action_dispatcher;
    using ingester_type = ingester;
    using store_type = store;
    using selection_type = selection;
    using drawers_type = drawer_registry;

    auto init() -> bool;
    auto uninit() -> void;

    auto update() -> void;
    auto draw() -> void;

    template<typename T, typename... Args>
    requires std::derived_from<T, extension>
    auto add_extension(Args&&... args) -> T&;
    auto add_extension(std::unique_ptr<extension> ext) -> extension&;

    template<typename T>
    requires std::derived_from<T, extension>
    auto remove_extension() -> bool;
    auto remove_extension(extension_id id) -> bool;

    template<typename T>
    requires std::derived_from<T, extension>
    auto has_extension() const -> bool;
    auto has_extension(extension_id id) const -> bool;

    template<typename T>
    requires std::derived_from<T, extension>
    auto find_extension() const -> const T*;
    auto find_extension(extension_id id) const -> const extension*;

    template<typename T>
    requires std::derived_from<T, extension>
    auto find_extension() -> T*;
    auto find_extension(extension_id id) -> extension*;

    auto store() noexcept -> store_type&;
    auto ingester() noexcept -> ingester_type&;
    auto selection() noexcept -> selection_type&;
    auto drawers() noexcept -> drawers_type&;

private:
    using sink_items = std::vector<item>;
    using extensions = std::vector<extension*>;
    using extension_by_ids = std::unordered_map<extension_id, std::unique_ptr<extension>>;

    auto drain_items() -> void;
    auto dispatch_actions() -> void;
    auto update_extensions() -> void;

    extensions _extensions;
    extension_by_ids _extension_by_ids;

    store_type _store;
    ingester_type _ingester;
    selection_type _selection;
    drawers_type _drawers;

    action_dispatcher_type _action_dispatcher;

    sink_items _sink_items;
};

template<typename T, typename... Args>
requires std::derived_from<T, extension>
auto host::add_extension(Args&&... args) -> T&
{
    return static_cast<T&>(add_extension(std::make_unique<T>(std::forward<Args>(args)...)));
}

template<typename T>
requires std::derived_from<T, extension>
auto host::remove_extension() -> bool
{
    return remove_extension(T::static_id);
}

template<typename T>
requires std::derived_from<T, extension>
auto host::has_extension() const -> bool
{
    return find_extension<T>() != nullptr;
}

template<typename T>
requires std::derived_from<T, extension>
auto host::find_extension() const -> const T*
{
    return static_cast<const T*>(find_extension(T::static_id));
}

template<typename T>
requires std::derived_from<T, extension>
auto host::find_extension() -> T*
{
    return static_cast<T*>(find_extension(T::static_id));
}

inline auto host::store() noexcept -> store_type&
{
    return _store;
}

inline auto host::ingester() noexcept -> ingester_type&
{
    return _ingester;
}

inline auto host::selection() noexcept -> selection_type&
{
    return _selection;
}

inline auto host::drawers() noexcept -> drawers_type&
{
    return _drawers;
}

} // namespace poc::workspace
