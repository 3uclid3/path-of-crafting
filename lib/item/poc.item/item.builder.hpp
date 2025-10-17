#pragma once

#include <poc.core/container/static_vector.hpp>
#include <poc.item/item.hpp>
#include <poc.item/item_blob.hpp>
#include <poc.item/modifier_kind.hpp>

namespace poc {

class item::builder
{
public:
    auto begin(item_blob&& blob) noexcept -> item::builder&;

    auto set_kind(item_kind kind) noexcept -> builder&;
    auto set_rarity(item_rarity rarity) noexcept -> builder&;

    auto set_name(string_view name) noexcept -> builder&;
    auto set_base_name(string_view base_name) noexcept -> builder&;

    auto begin_modifier(modifier_kind kind) noexcept -> builder&;
    auto set_modifier_name(string_view name) noexcept -> builder&;
    auto set_modifier_tier(modifier_tier tier) noexcept -> builder&;
    auto set_modifier_tags(string_view tags) noexcept -> builder&;
    auto set_modifier_lines(string_view line) noexcept -> builder&;
    auto end_modifier() noexcept -> builder&;

    [[nodiscard]] auto kind() const noexcept -> item_kind;
    [[nodiscard]] auto rarity() const noexcept -> item_rarity;

    [[nodiscard]] auto name() const noexcept -> string_view;
    [[nodiscard]] auto base_name() const noexcept -> string_view;

    [[nodiscard]] auto blob() const noexcept -> const item_blob&;

    [[nodiscard]] auto build() -> item;

private:
    struct raw_modifier
    {
        string_view name;
        string_view lines;
        string_view tags;
        modifier_tier tier;
        modifier_kind kind;
    };

    item_blob _blob;

    item_kind _kind{item_kind::unknown};
    item_rarity _rarity{item_rarity::unknown};

    string_view _name;
    string_view _base_name;

    static_vector<raw_modifier, 16> _modifiers;
    raw_modifier* _current_modifier{nullptr};
};

inline auto item::builder::set_kind(item_kind kind) noexcept -> builder&
{
    _kind = kind;
    return *this;
}

inline auto item::builder::set_rarity(item_rarity rarity) noexcept -> builder&
{
    _rarity = rarity;
    return *this;
}

inline auto item::builder::set_name(string_view name) noexcept -> builder&
{
    _name = name;
    return *this;
}

inline auto item::builder::set_base_name(string_view base_name) noexcept -> builder&
{
    _base_name = base_name;
    return *this;
}

inline auto item::builder::kind() const noexcept -> item_kind
{
    return _kind;
}

inline auto item::builder::rarity() const noexcept -> item_rarity
{
    return _rarity;
}

inline auto item::builder::name() const noexcept -> string_view
{
    return _name;
}

inline auto item::builder::base_name() const noexcept -> string_view
{
    return _base_name;
}

inline auto item::builder::blob() const noexcept -> const item_blob&
{
    return _blob;
}

inline auto item::builder::set_modifier_name(string_view name) noexcept -> builder&
{
    POC_ASSERT(_current_modifier != nullptr);

    _current_modifier->name = name;
    return *this;
}

inline auto item::builder::set_modifier_tier(modifier_tier tier) noexcept -> builder&
{
    POC_ASSERT(_current_modifier != nullptr);

    _current_modifier->tier = tier;
    return *this;
}

inline auto item::builder::set_modifier_tags(string_view tags) noexcept -> builder&
{
    POC_ASSERT(_current_modifier != nullptr);

    _current_modifier->tags = tags;
    return *this;
}

inline auto item::builder::set_modifier_lines(string_view line) noexcept -> builder&
{
    POC_ASSERT(_current_modifier != nullptr);

    _current_modifier->lines = line;
    return *this;
}

} // namespace poc
