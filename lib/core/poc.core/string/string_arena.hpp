#pragma once

#include <array>
#include <atomic>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <string_view>

#include <poc.core/assert.hpp>

namespace poc {

struct string_arena_statistics
{
    using size_type = std::size_t;

    size_type page_count{0};
    size_type page_size{0};

    size_type allocation_count{0};
    size_type allocated_bytes{0};
};

template<typename Char, std::size_t PageSize>
class basic_string_arena
{
    struct page;

public:
    using view_type = std::basic_string_view<Char>;
    using size_type = typename view_type::size_type;

    class lease
    {
    public:
        using arena_type = basic_string_arena<Char, PageSize>;
        using view_type = typename arena_type::view_type;

        lease() noexcept = default;
        ~lease() noexcept;

        lease(lease&& other) noexcept;
        auto operator=(lease&& other) noexcept -> lease&;

        lease(const lease&) = delete;
        auto operator=(const lease&) -> lease& = delete;

        constexpr auto is_valid() const noexcept -> bool;
        constexpr auto view() const noexcept -> view_type;

        auto release() noexcept -> void;

    private:
        using page_type = typename arena_type::page;

        std::atomic<page_type*> _page{nullptr};
        view_type _view;

        friend arena_type;
    };

    static constexpr size_type page_size = PageSize;

    [[nodiscard]] auto allocate(view_type string) -> lease;

    [[nodiscard]] auto statistics() const -> string_arena_statistics;

private:
    using page_buffer = std::array<Char, page_size>;

    struct page
    {
        using arena_type = basic_string_arena<Char, page_size>;

        arena_type& arena;

        page_buffer buffer{};

        size_type cursor{0};

        size_type allocation_count{0};
        size_type allocated_size{0};

        std::unique_ptr<page> previous{nullptr};
        page* next{nullptr};
    };

    auto release(page& p, std::size_t bytes) noexcept -> void;

private:
    mutable std::mutex _mutex;
    std::unique_ptr<page> _current_page{std::make_unique<page>(*this)};
};

template<typename Char, std::size_t PageSize>
auto basic_string_arena<Char, PageSize>::allocate(view_type string) -> lease
{
    POC_ASSERT(!string.empty());

    std::lock_guard lock{_mutex};

    const auto string_size = string.size();

    POC_ASSERT(string_size <= page_size);

    if (_current_page->cursor + string_size > page_size)
    {
        auto new_page = std::make_unique<page>(*this);

        new_page->previous = std::move(_current_page);
        new_page->previous->next = new_page.get();

        _current_page = std::move(new_page);
    }

    auto& page = *_current_page;

    const auto offset = page.cursor;
    std::memcpy(page.buffer.data() + offset, string.data(), string_size * sizeof(Char));

    page.cursor += string_size;

    page.allocation_count += 1;
    page.allocated_size += string_size;

    lease lease;
    lease._page = &page;
    lease._view = view_type{page.buffer.data() + offset, string_size};
    return lease;
}

template<typename Char, std::size_t PageSize>
auto basic_string_arena<Char, PageSize>::statistics() const -> string_arena_statistics
{
    std::lock_guard lock{_mutex};

    string_arena_statistics stats;
    stats.page_size = page_size;

    for (auto* p = _current_page.get(); p != nullptr; p = p->previous.get())
    {
        stats.page_count += 1;
        stats.allocation_count += p->allocation_count;
        stats.allocated_bytes += p->allocated_size * sizeof(Char);
    }

    return stats;
}

template<typename Char, std::size_t PageSize>
auto basic_string_arena<Char, PageSize>::release(page& p, std::size_t bytes) noexcept -> void
{
    std::lock_guard lock{_mutex};

    POC_ASSERT(p.allocated_size >= bytes);
    POC_ASSERT(p.allocation_count > 0);

    p.allocated_size -= bytes;
    p.allocation_count -= 1;

    if (p.allocation_count == 0)
    {
        POC_ASSERT(p.allocated_size == 0);

        if (p.previous)
        {
            p.previous->next = p.next;
        }

        if (p.next)
        {
            // "p" is destroyed here
            p.next->previous = std::move(p.previous);
        }
        else if (p.previous)
        {
            _current_page = std::move(p.previous);
        }
        else
        {
            POC_ASSERT(_current_page.get() == &p);

            _current_page->cursor = 0;
        }
    }
}

template<typename Char, std::size_t PageSize>
basic_string_arena<Char, PageSize>::lease::~lease() noexcept
{
    release();
}

template<typename Char, std::size_t PageSize>
basic_string_arena<Char, PageSize>::lease::lease(lease&& other) noexcept
    : _page(other._page.exchange(nullptr))
    , _view(std::exchange(other._view, {}))
{
}

template<typename Char, std::size_t PageSize>
auto basic_string_arena<Char, PageSize>::lease::operator=(lease&& other) noexcept -> lease&
{
    if (this != &other)
    {
        release();

        _page = other._page.exchange(nullptr);
        _view = std::exchange(other._view, {});
    }

    return *this;
}

template<typename Char, std::size_t PageSize>
constexpr auto basic_string_arena<Char, PageSize>::lease::is_valid() const noexcept -> bool
{
    return _page.load(std::memory_order_acquire) != nullptr;
}

template<typename Char, std::size_t PageSize>
constexpr auto basic_string_arena<Char, PageSize>::lease::view() const noexcept -> view_type
{
    return _view;
}

template<typename Char, std::size_t PageSize>
auto basic_string_arena<Char, PageSize>::lease::release() noexcept -> void
{
    if (page_type* page = _page.exchange(nullptr))
    {
        const auto size = _view.size();

        _view = {};

        page->arena.release(*page, size);
    }
}

} // namespace poc
