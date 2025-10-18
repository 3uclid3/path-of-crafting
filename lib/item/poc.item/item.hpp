#pragma once

#include <format>

#include <poc.core/container/static_vector.hpp>
#include <poc.core/types.hpp>
#include <poc.item/item_blob.hpp>
#include <poc.item/modifier.hpp>

namespace poc {

enum class item_rarity : u8
{
    unknown,
    normal,
    magic,
    rare,
    unique
};

enum class item_kind : u8
{
    unknown
};

class item
{
public:
    class builder;

    using blob_type = item_blob;

    using implicits_type = static_vector<modifier, 4>;
    using prefixes_type = static_vector<modifier, 3>;
    using suffixes_type = static_vector<modifier, 3>;

    constexpr item() noexcept = default;

    constexpr item(item&&) noexcept = default;
    constexpr item& operator=(item&&) noexcept = default;

    constexpr item(const item&) = delete;
    constexpr item& operator=(const item&) = delete;

    constexpr auto kind() const noexcept -> item_kind { return _kind; }
    constexpr auto rarity() const noexcept -> item_rarity { return _rarity; }

    constexpr auto name() const noexcept -> string_view { return _name; }
    constexpr auto base_name() const noexcept -> string_view { return _base_name; }

    constexpr auto implicits() const noexcept -> const implicits_type& { return _implicits; }
    constexpr auto prefixes() const noexcept -> const prefixes_type& { return _prefixes; }
    constexpr auto suffixes() const noexcept -> const suffixes_type& { return _suffixes; }

    constexpr auto blob() const noexcept -> string_view { return _blob.view(); }

private:
    blob_type _blob;

    item_kind _kind{item_kind::unknown};
    item_rarity _rarity{item_rarity::unknown};

    string_view _name;
    string_view _base_name;

    implicits_type _implicits;
    prefixes_type _prefixes;
    suffixes_type _suffixes;
};

} // namespace poc

template<>
struct std::formatter<poc::item_kind> : std::formatter<std::string_view>
{
    constexpr auto format(poc::item_kind k, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(magic_enum::enum_name(k), ctx);
    }
};

template<>
struct std::formatter<poc::item_rarity> : std::formatter<std::string_view>
{
    constexpr auto format(poc::item_rarity r, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(magic_enum::enum_name(r), ctx);
    }
};

template<>
struct std::formatter<poc::item> : std::formatter<std::string_view>
{
    constexpr auto format(const poc::item& item, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(item.blob(), ctx);
    }
};
