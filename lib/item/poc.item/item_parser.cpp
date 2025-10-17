#include <poc.item/item_parser.hpp>

#include <charconv>

#include <poc.core/algorithm.hpp>
#include <poc.core/def.hpp>
#include <poc.item/item.hpp>
#include <poc.item/item_line_index.hpp>
#include <poc.item/log.hpp>

namespace poc {

constexpr item_parser::state::function_table item_parser::state::base_table = [] {
    function_table table{};
    table.fill(&invoke_then_tail_dispatch<parse_unexpected>);

    // ensure we stop on end of blob
    table[std::to_underlying(item_line_index::line_kind::end)] = &parse_unexpected;

    // dividers are handled specially
    table[std::to_underlying(item_line_index::line_kind::divider)] = &invoke_then_tail_dispatch<parse_divider>;

    return table;
}();

constexpr item_parser::state::function_table item_parser::state::parse_section::table{[] {
    function_table table{base_table};

    table[std::to_underlying(item_line_index::line_kind::key_value)] = &invoke_then_tail_dispatch<parse_key_value>;
    table[std::to_underlying(item_line_index::line_kind::braced)] = &invoke_then_tail_dispatch<parse_braced>;
    table[std::to_underlying(item_line_index::line_kind::text)] = &invoke_then_tail_dispatch<parse_text>;

    return table;
}()};

constexpr item_parser::state::function_table item_parser::state::parse_header::table{[] {
    function_table table{base_table};

    table[std::to_underlying(item_line_index::line_kind::key_value)] = &invoke_then_tail_dispatch<parse_key_value>;
    table[std::to_underlying(item_line_index::line_kind::text)] = &invoke_then_tail_dispatch<parse_text>;

    return table;
}()};

constexpr item_parser::state::function_table item_parser::state::parse_armor::table{[] {
    function_table table{base_table};

    // table[std::to_underlying(item_line_index::line_kind::key_value)] = &invoke_then_tail_dispatch<parse_key_value>;

    return table;
}()};

constexpr item_parser::state::function_table item_parser::state::parse_weapon::table{[] {
    function_table table{base_table};

    // table[std::to_underlying(item_line_index::line_kind::key_value)] = &invoke_then_tail_dispatch<parse_key_value>;

    return table;
}()};

constexpr item_parser::state::function_table item_parser::state::parse_requirements::table{[] {
    function_table table{base_table};

    // table[std::to_underlying(item_line_index::line_kind::key_value)] = &invoke_then_tail_dispatch<parse_key_value>;
    // table[std::to_underlying(item_line_index::line_kind::text)] = &invoke_then_tail_dispatch<parse_text>;

    return table;
}()};

constexpr item_parser::state::function_table item_parser::state::parse_sockets::table{[] {
    function_table table{base_table};

    // table[std::to_underlying(item_line_index::line_kind::key_value)] = &invoke_then_tail_dispatch<parse_key_value>;
    // table[std::to_underlying(item_line_index::line_kind::text)] = &invoke_then_tail_dispatch<parse_text>;

    return table;
}()};

constexpr item_parser::state::function_table item_parser::state::parse_item_level::table{[] {
    function_table table{base_table};

    // table[std::to_underlying(item_line_index::line_kind::key_value)] = &invoke_then_tail_dispatch<parse_key_value>;
    // table[std::to_underlying(item_line_index::line_kind::text)] = &invoke_then_tail_dispatch<parse_text>;

    return table;
}()};

constexpr item_parser::state::function_table item_parser::state::parse_implicits::table{[] {
    function_table table{base_table};

    // table[std::to_underlying(item_line_index::line_kind::key_value)] = &invoke_then_tail_dispatch<parse_key_value>;
    // table[std::to_underlying(item_line_index::line_kind::text)] = &invoke_then_tail_dispatch<parse_text>;

    return table;
}()};

constexpr item_parser::state::function_table item_parser::state::parse_explicits::table{[] {
    function_table table{base_table};

    table[std::to_underlying(item_line_index::line_kind::braced)] = &invoke_then_tail_dispatch<parse_braced>;
    table[std::to_underlying(item_line_index::line_kind::text)] = &invoke_then_tail_dispatch<parse_text>;

    return table;
}()};

constexpr item_parser::state::function_table item_parser::state::parse_unknown::table{[] {
    function_table table{base_table};

    // table[std::to_underlying(item_line_index::line_kind::key_value)] = &invoke_then_tail_dispatch<parse_key_value>;
    // table[std::to_underlying(item_line_index::line_kind::text)] = &invoke_then_tail_dispatch<parse_text>;

    return table;
}()};

auto item_parser::can_parse(string_view raw) const noexcept -> bool
{
    if (!raw.starts_with("Item Class"))
        return false;

    if (ranges::count_until(raw, '\n', [](auto count) { return count >= 4; }) < 4)
        return false;

    return true;
}

auto item_parser::parse(item_blob&& blob) -> std::expected<item, error>
{
    _line_index = item_line_index{blob.view()};

    if (_line_index.size() <= 1)
    {
        return std::unexpected(error::empty);
    }

    _cursor = _line_index.begin();
    _builder.begin(std::move(blob));

    transition_to<state::parse_header>();

    tail_dispatch(*this);

    return _builder.build();
}

auto item_parser::state::parse_section::parse_key_value(item_parser& self) -> void
{
    const auto line = self.line();
    const auto key = line.key();

    if (key == "Sockets")
    {
        self.transition_to<parse_sockets>();
    }
    else if (key == "Item Level")
    {
        self.transition_to<parse_item_level>();
    }
    else
    {
        self.transition_to<parse_unknown>();
    }
}

auto item_parser::state::parse_section::parse_braced(item_parser& self) -> void
{
    const auto line = self.line();
    const auto content = line.unbraced();

    if (content.starts_with("Implicit"))
    {
        self.transition_to<parse_implicits>();
    }
    else if (content.starts_with("Prefix") || content.starts_with("Suffix"))
    {
        self.transition_to<parse_explicits>();
    }
    else
    {
        self.transition_to<parse_unknown>();
    }
}

auto item_parser::state::parse_section::parse_text(item_parser& self) -> void
{
    const auto line = self.line();
    const auto raw_line = line.raw();

    if (raw_line.starts_with("Requirements"))
    {
        self.transition_to<parse_requirements>();
    }
    else
    {
        self.transition_to<parse_unknown>();
    }
}

auto item_parser::state::parse_header::parse_key_value(item_parser& self) -> void
{
    const auto line = self.line();

    const auto key = line.key();
    const auto value = line.value();

    if (key == "Item Class")
    {
        parse_kind(self, value);
    }
    else if (key == "Rarity")
    {
        parse_rarity(self, value);
    }
    else
    {
        POC_ITEM_ERROR("Unknown key/value pair {}: {}", key, value);
    }

    self.consume_line();
}

auto item_parser::state::parse_header::parse_text(item_parser& self) -> void
{
    auto& builder = self._builder;
    const auto raw_line = self.line().raw();

    switch (builder.rarity())
    {
    case item_rarity::normal:
        builder.set_name(raw_line);
        builder.set_base_name(raw_line);
        break;

    case item_rarity::magic:
        builder.set_name(raw_line);
        builder.set_base_name(raw_line);
        break;

    case item_rarity::rare:
        if (builder.name().empty())
            builder.set_name(raw_line);
        else if (builder.base_name().empty())
            builder.set_base_name(raw_line);
        else
            POC_ITEM_ERROR("Unexpected extra text line for rare item header: {}", raw_line);
        break;
    case item_rarity::unique:
        // Handle unique rarity text
        break;

    case item_rarity::unknown:
        break;
    }

    self.consume_line();
}

auto item_parser::state::parse_header::parse_rarity(item_parser& self, string_view value) -> void
{
    if (value == "Normal")
    {
        self._builder.set_rarity(item_rarity::normal);
    }
    else if (value == "Magic")
    {
        self._builder.set_rarity(item_rarity::magic);
    }
    else if (value == "Rare")
    {
        self._builder.set_rarity(item_rarity::rare);
    }
    else if (value == "Unique")
    {
        self._builder.set_rarity(item_rarity::unique);
    }
    else
    {
        POC_ITEM_ERROR("Unknown item rarity: {}", value);
    }
}

auto item_parser::state::parse_header::parse_kind(item_parser&, string_view value [[maybe_unused]]) -> void
{
    POC_ITEM_WARN("Unknown item kind: {}", value);
}

// Format:
// { Prefix|Suffix Modifier "<name>" (Rank|Tier: <rank|tier>) — <tags> }
auto item_parser::state::parse_explicits::parse_braced(item_parser& self) -> void
{
    const auto line = self.line();
    const auto content = line.unbraced();

    const auto report_invalid_and_consume = [&]() {
        POC_ITEM_ERROR("Invalid explicit format: {}", content);
        self.consume_line();
    };

    std::size_t modifier_kind_length = 0;
    if (content.starts_with("Prefix Modifier"))
    {
        self._builder.begin_modifier(modifier_kind::prefix);
        modifier_kind_length = sizeof("Prefix Modifier") - 1;
    }
    else if (content.starts_with("Suffix Modifier"))
    {
        self._builder.begin_modifier(modifier_kind::suffix);
        modifier_kind_length = sizeof("Suffix Modifier") - 1;
    }
    else
    {
        return report_invalid_and_consume();
    }

    const auto name_start = content.find('"', modifier_kind_length);
    if (name_start == std::string_view::npos)
    {
        return report_invalid_and_consume();
    }

    const auto name_end = content.find('"', name_start + 1);
    if (name_end == std::string_view::npos)
    {
        return report_invalid_and_consume();
    }

    self._builder.set_modifier_name(content.substr(name_start + 1, name_end - name_start - 1));

    bool is_rank = false;

    auto tier_start = content.find("Tier:", name_end + 1);
    if (tier_start == std::string_view::npos)
    {
        tier_start = content.find("Rank:", name_end + 1);
        if (tier_start == std::string_view::npos)
        {
            return report_invalid_and_consume();
        }

        is_rank = true;
    }

    tier_start += 6; // after "Tier: " or "Rank: "

    auto tier_end = content.find(' ', tier_start);
    if (tier_end == std::string_view::npos)
    {
        return report_invalid_and_consume();
    }

    std::uint8_t tier_value = 0;
    if (std::from_chars(content.data() + tier_start, content.data() + tier_end, tier_value).ec != std::errc{})
    {
        return report_invalid_and_consume();
    }

    self._builder.set_modifier_tier(is_rank ? modifier_tier{tier_value, modifier_tier::rank_tag{}} : modifier_tier{tier_value});

    self.consume_line();
}

auto item_parser::state::parse_explicits::parse_text(item_parser& self) -> void
{
    auto line = self.line();

    const auto line_start = line.raw().data();
    auto line_end = line_start + line.raw().size();
    self.consume_line();

    // consume all contiguous text lines
    for (line = self.line(); line.kind() == item_line_index::line_kind::text; line = self.line())
    {
        line_end = line.raw().data() + line.raw().size();
        self.consume_line();
    }

    self._builder.set_modifier_lines(string_view{line_start, static_cast<std::size_t>(line_end - line_start)});
}

template<auto Function>
auto item_parser::state::invoke_then_tail_dispatch(item_parser& self) -> void
{
    std::invoke(Function, self);

    POC_MUSTTAIL return tail_dispatch(self);
}

auto item_parser::state::parse_unexpected(item_parser& self) -> void
{
    self.consume_line();
}

auto item_parser::state::parse_divider(item_parser& self) -> void
{
    self.consume_line();
    self.transition_to<parse_section>();
}

auto item_parser::tail_dispatch(item_parser& self) -> void
{
    const std::size_t index = static_cast<std::size_t>((*self._cursor).kind());

    POC_ASSERT(index < self._table->size());
    POC_ASSERT((*self._table)[index] != nullptr);

    POC_MUSTTAIL return (*self._table)[index](self);
}

template<typename State>
auto item_parser::transition_to() noexcept -> void
{
    _table = &State::table;
}

auto item_parser::consume_line() noexcept -> void
{
    ++_cursor;
}

auto item_parser::line() const noexcept -> item_line_index::line
{
    return *_cursor;
}

} // namespace poc
