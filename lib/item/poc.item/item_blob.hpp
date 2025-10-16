#pragma once

#include <format>
#include <functional>
#include <variant>
#include <vector>

#include <poc.core/types.hpp>

namespace poc {

class item_blob
{
public:
    using view_type = string_view;
    using value_type = view_type::value_type;
    using size_type = view_type::size_type;

    using string_buffer_type = std::vector<value_type>;
    using string_lease_type = string_lease;
    using string_view_type = view_type;

    using storage_type = std::variant<string_lease_type, string_buffer_type>;

    using const_iterator = view_type::const_iterator;
    using iterator = const_iterator;

public:
    constexpr item_blob() noexcept = default;
    constexpr explicit item_blob(string_buffer_type raw) noexcept;
    constexpr explicit item_blob(string_lease_type raw) noexcept;
    constexpr explicit item_blob(string_view_type raw);

    constexpr item_blob(const item_blob&) = delete;
    constexpr item_blob& operator=(const item_blob&) = delete;

    constexpr item_blob(item_blob&&) noexcept = default;
    constexpr item_blob& operator=(item_blob&&) noexcept = default;

    constexpr auto empty() const noexcept -> bool;
    constexpr auto size() const noexcept -> size_type;

    constexpr auto operator[](size_type index) const noexcept -> value_type;

    constexpr auto begin() const noexcept -> const_iterator;
    constexpr auto end() const noexcept -> const_iterator;

    constexpr auto view() const noexcept -> view_type;

private:
    storage_type _storage{std::in_place_type_t<string_buffer_type>(), string_buffer_type()};
    view_type _view{std::get<string_buffer_type>(_storage)};
};

constexpr item_blob::item_blob(string_buffer_type raw) noexcept
    : _storage(std::in_place_type_t<string_buffer_type>(), std::move(raw))
    , _view(string_view_type(std::get<string_buffer_type>(_storage)))
{
}

constexpr item_blob::item_blob(string_lease_type raw) noexcept
    : _storage(std::in_place_type_t<string_lease_type>(), std::move(raw))
    , _view(std::get<string_lease_type>(_storage).view())
{
}

constexpr item_blob::item_blob(string_view_type raw)
    : _storage(std::in_place_type_t<string_buffer_type>(), string_buffer_type(std::begin(raw), std::end(raw)))
    , _view(string_view_type(std::get<string_buffer_type>(_storage)))
{
}

constexpr auto item_blob::empty() const noexcept -> bool
{
    return _view.empty();
}

constexpr auto item_blob::size() const noexcept -> size_type
{
    return _view.size();
}

constexpr auto item_blob::operator[](size_type index) const noexcept -> value_type
{
    return _view[index];
}

constexpr auto item_blob::begin() const noexcept -> const_iterator
{
    return _view.begin();
}

constexpr auto item_blob::end() const noexcept -> const_iterator
{
    return _view.end();
}

constexpr auto item_blob::view() const noexcept -> view_type
{
    return _view;
}

} // namespace poc

template<>
struct std::formatter<poc::item_blob> : std::formatter<poc::item_blob::view_type>
{
    constexpr auto format(const poc::item_blob& blob, std::format_context& ctx) const
    {
        return std::formatter<poc::item_blob::view_type>::format(blob.view(), ctx);
    }
};

template<>
struct std::hash<poc::item_blob>
{
    std::size_t operator()(const poc::item_blob& blob) const noexcept
    {
        return std::hash<poc::item_blob::view_type>()(blob.view());
    }
};
