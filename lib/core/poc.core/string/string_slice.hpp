#pragma once

#include <string_view>

#include <poc.core/assert.hpp>

namespace poc {

template<typename Char>
class basic_string_slice
{
public:
    using view_type = std::basic_string_view<Char>;
    using size_type = typename view_type::size_type;

    constexpr basic_string_slice() noexcept = default;
    constexpr basic_string_slice(size_type offset, size_type size) noexcept;

    constexpr auto offset() const noexcept -> size_type;
    constexpr auto size() const noexcept -> size_type;

    constexpr auto view(view_type string) const noexcept -> view_type;

private:
    size_type _offset{0};
    size_type _size{0};
};

template<typename Char>
constexpr basic_string_slice<Char>::basic_string_slice(size_type offset, size_type size) noexcept
    : _offset(offset)
    , _size(size)
{
}

template<typename Char>
constexpr auto basic_string_slice<Char>::offset() const noexcept -> size_type
{
    return _offset;
}

template<typename Char>
constexpr auto basic_string_slice<Char>::size() const noexcept -> size_type
{
    return _size;
}

template<typename Char>
constexpr auto basic_string_slice<Char>::view(view_type string) const noexcept -> view_type
{
    POC_ASSERT(_offset + _size <= string.size());
    return string.substr(_offset, _size);
}

} // namespace poc
