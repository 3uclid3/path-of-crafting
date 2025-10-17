#include <poc.item/item_line_index.hpp>

#include <algorithm>

#include <poc.core/assert.hpp>

namespace poc {

item_line_index::item_line_index(std::string_view raw)
    : _raw(raw)
{
    parse();
}

auto item_line_index::operator[](size_type index) const noexcept -> line
{
    POC_ASSERT(index < _entries.size());

    const auto next_index = index + 1;

    if (next_index == _entries.size())
    {
        return line({}, _entries.back());
    }

    const auto& entry = _entries[index];
    const auto& entry_next = _entries[next_index];

    const size_type start = static_cast<size_type>(entry.offset);
    const size_type next_start = static_cast<size_type>(entry_next.offset);

    const size_type end_trim = (entry.has_cr ? 1 : 0) + (entry.has_lf ? 1 : 0); // trim \n and \r\n

    POC_ASSERT(start < next_start);
    POC_ASSERT((start + end_trim) <= next_start);

    const size_type length = (next_start - start) - end_trim;

    return line(_raw.substr(start, length), entry);
}

auto item_line_index::parse() -> void
{
    const std::size_t line_count = std::count(_raw.begin(), _raw.end(), '\n') + 1;

    _entries.reserve(line_count + 1); // +1 for end entry

    std::size_t line_start = 0;

    for (std::size_t i = 0; i < line_count && line_start < _raw.size(); ++i)
    {
        std::size_t line_end = _raw.find('\n', line_start);
        line_end = line_end == string_view::npos ? _raw.size() : line_end + 1;

        std::string_view line = _raw.substr(line_start, line_end - line_start);

        parse_line(line, static_cast<size_type>(line_start));

        line_start = line_end;
    }

    _entries.emplace_back();

    line_entry& entry = _entries.back();
    entry.kind = static_cast<std::uint32_t>(line_kind::end);
    entry.offset = static_cast<size_type>(_raw.size());
}

auto item_line_index::parse_line(std::string_view line, size_type offset) -> void
{
    POC_ASSERT(!line.empty());

    const bool has_lf = line.back() == '\n';
    const bool has_cr = line.size() > 1 && line[line.size() - 2] == '\r';

    _entries.emplace_back();

    line_entry& entry = _entries.back();
    entry.offset = offset;
    entry.has_cr = has_cr ? 1 : 0;
    entry.has_lf = has_lf ? 1 : 0;

    if (has_lf)
    {
        line.remove_suffix(1);
    }

    if (has_cr)
    {
        line.remove_suffix(1);
    }

    if (line.empty())
    {
        entry.kind = static_cast<std::uint32_t>(line_kind::empty);
    }
    else if (line == "--------")
    {
        entry.kind = static_cast<std::uint32_t>(line_kind::divider);
    }
    else if (line.front() == '{' && line.back() == '}')
    {
        entry.kind = static_cast<std::uint32_t>(line_kind::braced);

        const bool has_inner_space = line.size() >= 4 && line[1] == ' ' && line[line.size() - 2] == ' ';
        entry.payload = has_inner_space ? 1 : 0;
    }
    else
    {
        const std::size_t colon_pos = line.find(": ");

        if (colon_pos != std::string_view::npos)
        {
            entry.kind = static_cast<std::uint32_t>(line_kind::key_value);
            entry.payload = static_cast<std::uint32_t>(colon_pos + 2); // +2 to skip ": "
        }
        else
        {
            entry.kind = static_cast<std::uint32_t>(line_kind::text);
        }
    }
}

} // namespace poc
