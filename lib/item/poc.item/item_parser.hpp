#pragma once

#include <array>
#include <expected>

#include <poc.core/types.hpp>
#include <poc.item/item.builder.hpp>
#include <poc.item/item.hpp>
#include <poc.item/item_blob.hpp>
#include <poc.item/item_line_index.hpp>

namespace poc {

class item_parser
{
public:
    enum class error
    {
        empty
    };

    [[nodiscard]] auto can_parse(string_view raw) const noexcept -> bool;
    [[nodiscard]] auto parse(item_blob&& blob) -> std::expected<item, error>;

private:
    // state are represented as function tables indexed by line_kind
    // each function is responsible for consuming lines and transitioning to the next state
    struct state
    {
        using function = void (*)(item_parser&);
        using function_table = std::array<function, magic_enum::enum_count<item_line_index::line_kind>()>;

        // after a divider, determine the section type by the first line
        // then transition to the appropriate section parser
        struct parse_section
        {
            static auto parse_key_value(item_parser& self) -> void;
            static auto parse_braced(item_parser& self) -> void;
            static auto parse_text(item_parser& self) -> void;

            static const function_table table;
        };

        // the initial state, parse item name, rarity, kind
        struct parse_header
        {
            static auto parse_key_value(item_parser& self) -> void;
            static auto parse_text(item_parser& self) -> void;

            static auto parse_rarity(item_parser& self, string_view value) -> void;
            static auto parse_kind(item_parser& self, string_view value) -> void;

            static const function_table table;
        };

        struct parse_armor
        {
            static auto parse_key_value(item_parser& self) -> void;

            static const function_table table;
        };

        struct parse_weapon
        {
            static auto parse_key_value(item_parser& self) -> void;

            static const function_table table;
        };

        struct parse_requirements
        {
            static auto parse_key_value(item_parser& self) -> void;
            static auto parse_text(item_parser& self) -> void;

            static const function_table table;
        };

        struct parse_sockets
        {
            static auto parse_key_value(item_parser& self) -> void;

            static const function_table table;
        };

        struct parse_item_level
        {
            static auto parse_key_value(item_parser& self) -> void;

            static const function_table table;
        };

        struct parse_modifiers
        {
            static auto parse_tier(string_view truncated) -> modifier_tier;
        };

        struct parse_implicits : parse_modifiers
        {
            // start a new implicit
            static auto parse_braced(item_parser& self) -> void;

            // add a line to the current implicit
            static auto parse_text(item_parser& self) -> void;

            static const function_table table;
        };

        struct parse_explicits : parse_modifiers
        {
            // start a new prefix or suffix, contains name, rank/tier, tags
            static auto parse_braced(item_parser& self) -> void;

            // add a line to the current prefix or suffix
            static auto parse_text(item_parser& self) -> void;

            static const function_table table;
        };

        struct parse_unknown
        {
            static auto parse_key_value(item_parser& self) -> void;
            static auto parse_text(item_parser& self) -> void;

            static const function_table table;
        };

        template<auto Function>
        static auto invoke_then_tail_dispatch(item_parser& self) -> void;

        static auto parse_unexpected(item_parser& self) -> void;
        static auto parse_divider(item_parser& self) -> void;

        static const function_table base_table;
    };

    static auto tail_dispatch(item_parser& self) -> void;

    template<typename State>
    auto transition_to() noexcept -> void;

    auto consume_line() noexcept -> void;

    auto line() const noexcept -> item_line_index::line;

    const state::function_table* _table{&state::parse_header::table};

    item::builder _builder;
    item_line_index _line_index;
    item_line_index::const_iterator _cursor{_line_index.begin()};
};

} // namespace poc
