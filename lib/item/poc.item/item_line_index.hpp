#pragma once

#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include <poc.core/assert.hpp>
#include <poc.core/types.hpp>
#include <poc.item/item_blob.hpp>

namespace poc {

class item_line_index
{
public:
    using size_type = std::uint32_t;

    enum class line_kind : std::uint8_t
    {
        // sentinel, keep at value 0
        end,

        // --------
        divider,

        // key: value
        key_value,

        // { ... }
        braced,

        // anything else
        text,

        // empty line (only newline)
        empty,
    };

    class line;
    class const_iterator;

public:
    item_line_index() noexcept = default;

    item_line_index(string_view raw);

    item_line_index(item_line_index&& other) noexcept = default;
    auto operator=(item_line_index&& other) -> item_line_index& = default;

    item_line_index(const item_line_index& other) = delete;
    auto operator=(const item_line_index& other) -> item_line_index& = delete;

    auto operator[](size_type index) const noexcept -> line;
    auto at(size_type index) const -> line;

    auto size() const noexcept -> size_type;
    auto empty() const noexcept -> bool;

    auto begin() const noexcept -> const_iterator;
    auto end() const noexcept -> const_iterator;

    auto raw() const noexcept -> string_view;

private:
    auto parse() -> void;
    auto parse_line(string_view line, size_type offset) -> void;

private:
    struct line_entry
    {
        // payload per kind:
        // - end: none
        // - divider: none
        // - key_value: offset of value start (from byte_offset)
        // - braced: 0 or 1 if there is a space after '{' and before '}'
        // - text: none

        static constexpr int kind_bits = sizeof(line_kind) * 8;
        static constexpr int lf_bits = 1;
        static constexpr int cr_bits = 1;
        static constexpr int payload_bits = 22;

        std::uint32_t kind : kind_bits{0};
        std::uint32_t has_lf : lf_bits{0};
        std::uint32_t has_cr : cr_bits{0};
        std::uint32_t payload : payload_bits{0};
        std::uint32_t offset{0};
    };
    static_assert(sizeof(line_entry) == 8, "line_entry must be 8-byte");

    string_view _raw;
    std::vector<line_entry> _entries;
};

using item_line = item_line_index::line;

class item_line_index::line
{
public:
    line() noexcept = default;
    line(string_view raw, line_entry entry) noexcept;

    auto has_lf() const noexcept -> bool;
    auto has_cr() const noexcept -> bool;

    auto kind() const noexcept -> line_kind;
    auto offset() const noexcept -> size_type;

    // only for key_value
    auto key() const noexcept -> string_view;
    auto value() const noexcept -> string_view;
    auto value_offset() const noexcept -> size_type;

    // only for braced
    auto unbraced() const noexcept -> string_view;

    // without CR/LF
    auto raw() const noexcept -> string_view;

private:
    string_view _raw;
    line_entry _entry;
};

class item_line_index::const_iterator
{
public:
    using value_type = line;

public:
    const_iterator(const item_line_index& source, size_type index) noexcept;

    const_iterator(const const_iterator& other) noexcept = default;
    auto operator=(const const_iterator& other) -> const_iterator& = default;

    const_iterator(const_iterator&& other) noexcept = default;
    auto operator=(const_iterator&& other) -> const_iterator& = default;

    auto operator==(const const_iterator& other) const noexcept -> bool;

    auto operator++() noexcept -> const_iterator&;
    auto operator++(int) noexcept -> const_iterator;

    auto operator*() const noexcept -> value_type;

private:
    std::reference_wrapper<const item_line_index> _source;
    size_type _index{0};
};

inline item_line_index::line::line(string_view raw, line_entry entry) noexcept
    : _raw(raw)
    , _entry(entry)
{
}

inline auto item_line_index::line::has_lf() const noexcept -> bool
{
    return _entry.has_lf != 0;
}

inline auto item_line_index::line::has_cr() const noexcept -> bool
{
    return _entry.has_cr != 0;
}

inline auto item_line_index::line::kind() const noexcept -> line_kind
{
    return static_cast<line_kind>(_entry.kind);
}

inline auto item_line_index::line::offset() const noexcept -> size_type
{
    return static_cast<size_type>(_entry.offset);
}

inline auto item_line_index::line::key() const noexcept -> string_view
{
    POC_ASSERT(kind() == line_kind::key_value);
    return _raw.substr(0, value_offset() - 2); // -2 for ": "
}

inline auto item_line_index::line::value() const noexcept -> string_view
{
    POC_ASSERT(kind() == line_kind::key_value);
    return _raw.substr(value_offset());
}

inline auto item_line_index::line::value_offset() const noexcept -> size_type
{
    POC_ASSERT(kind() == line_kind::key_value);
    return static_cast<size_type>(_entry.payload);
}

inline auto item_line_index::line::unbraced() const noexcept -> string_view
{
    POC_ASSERT(kind() == line_kind::braced);

    const bool has_inner_space = _entry.payload != 0;

    // "{ ... }" or "{...}"
    const size_type start = has_inner_space ? 2 : 1;
    const size_type size = has_inner_space ? _raw.size() - 4 : _raw.size() - 2;

    return _raw.substr(start, size);
}

inline auto item_line_index::line::raw() const noexcept -> string_view
{
    return _raw;
}

inline item_line_index::const_iterator::const_iterator(const item_line_index& source, size_type index) noexcept
    : _source(source)
    , _index(index)
{
}

inline auto item_line_index::const_iterator::operator==(const const_iterator& other) const noexcept -> bool
{
    return &_source.get() == &other._source.get() && _index == other._index;
}

inline auto item_line_index::const_iterator::operator++() noexcept -> const_iterator&
{
    ++_index;
    return *this;
}

inline auto item_line_index::const_iterator::operator++(int) noexcept -> const_iterator
{
    const_iterator tmp = *this;
    ++(*this);
    return tmp;
}

inline auto item_line_index::const_iterator::operator*() const noexcept -> value_type
{
    return _source.get()[_index];
}

inline auto item_line_index::at(size_type index) const -> line
{
    if (index >= size())
    {
        throw std::out_of_range("item_line_index::at: index out of range");
    }

    return (*this)[index];
}

inline auto item_line_index::size() const noexcept -> size_type
{
    return static_cast<size_type>(_entries.size());
}

inline auto item_line_index::empty() const noexcept -> bool
{
    return size() == 0;
}

inline auto item_line_index::begin() const noexcept -> const_iterator
{
    return const_iterator(*this, 0);
}

inline auto item_line_index::end() const noexcept -> const_iterator
{
    return const_iterator(*this, size());
}

inline auto item_line_index::raw() const noexcept -> string_view
{
    return _raw;
}

} // namespace poc
