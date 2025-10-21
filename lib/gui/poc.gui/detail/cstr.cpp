#include <poc.gui/detail/cstr.hpp>

#include <bit>
#include <iterator>
#include <vector>

namespace poc::gui::detail {

namespace {

static auto next_buffer() -> std::vector<char>&
{
    static constexpr std::size_t max_slots = 16;
    static_assert(std::has_single_bit(max_slots), "must be power of two for bitmask wrap");

    thread_local std::array<std::vector<char>, max_slots> slots{[] {
        std::array<std::vector<char>, max_slots> arr{};
        for (auto& slot : arr) slot.reserve(256);
        return arr;
    }()};
    thread_local std::size_t i{0};

    return slots[(i++) & (max_slots - 1)];
}

} // namespace

auto to_cstr(string_view sv) -> const char*
{
    auto& buffer = next_buffer();
    buffer.clear();

    buffer.insert(buffer.end(), sv.begin(), sv.end());
    buffer.push_back('\0');

    return buffer.data();
}

auto vformat_cstr(string_view fmt, format_args args) -> const char*
{
    auto& buffer = next_buffer();
    buffer.clear();

    std::vformat_to(std::back_inserter(buffer), fmt, args);
    buffer.push_back('\0');

    return buffer.data();
}

} // namespace poc::gui::detail