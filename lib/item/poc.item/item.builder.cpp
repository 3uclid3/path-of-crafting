#include <poc.item/item.builder.hpp>

namespace poc {

namespace {

auto trim_prefix_base_name(string_view name) -> string_view
{
    // TODO trim any prefixes from magic items
    return name;
}

auto trim_suffix_base_name(string_view name) -> string_view
{
    const auto of = name.find(" of ");

    if (of != string_view::npos)
    {
        return name.substr(0, of);
    }

    return name;
}

} // namespace

auto item::builder::begin(item_blob&& blob) noexcept -> builder&
{
    _blob = std::move(blob);

    _kind = item_kind::unknown;
    _rarity = item_rarity::unknown;

    _name = {};
    _base_name = {};

    _modifiers.clear();
    _current_modifier = nullptr;

    return *this;
}

auto item::builder::build() -> item
{
    // ensure any open modifier is closed
    end_modifier();

    item result;
    result._blob = std::move(_blob);
    result._kind = _kind;
    result._rarity = _rarity;
    result._name = _name;
    result._base_name = _base_name;

    for (raw_modifier& raw_mod : _modifiers)
    {
        switch (raw_mod.kind)
        {
        case modifier_kind::implicit:
            result._implicits.emplace_back(raw_mod.name, raw_mod.lines, raw_mod.tags, raw_mod.tier);
            break;
        case modifier_kind::prefix:
            result._prefixes.emplace_back(raw_mod.name, raw_mod.lines, raw_mod.tags, raw_mod.tier);
            break;
        case modifier_kind::suffix:
            result._suffixes.emplace_back(raw_mod.name, raw_mod.lines, raw_mod.tags, raw_mod.tier);
            break;
        }
    }

    if (result._rarity == item_rarity::magic)
    {
        // todo get prefix name to trim
        result._base_name = trim_prefix_base_name(result._base_name);
        // todo get suffix name to trim
        result._base_name = trim_suffix_base_name(result._base_name);
    }

    return result;
}

auto item::builder::begin_modifier(modifier_kind kind) noexcept -> builder&
{
    _modifiers.emplace_back();

    _current_modifier = &_modifiers.back();
    _current_modifier->kind = kind;

    return *this;
}

auto item::builder::end_modifier() noexcept -> builder&
{
    if (_current_modifier != nullptr)
    {
        _current_modifier = nullptr;
    }
    return *this;
}

} // namespace poc
