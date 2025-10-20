#include <poc.gui/widgets.hpp>

#include <bit>
#include <format>
#include <iterator>
#include <vector>

#include <imgui.h>

namespace poc::gui {

inline auto next_buffer() -> std::vector<char>&
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

// until imgui supports string slice natively
inline auto imgui_cstr(string_view sv) -> const char*
{
    auto& buffer = next_buffer();
    buffer.clear();

    buffer.insert(buffer.end(), sv.begin(), sv.end());
    buffer.push_back('\0');

    return buffer.data();
}

inline string_view imgui_format_cstr(string_view fmt, format_args args)
{
    auto& buffer = next_buffer();
    buffer.clear();

    std::vformat_to(std::back_inserter(buffer), fmt, args);
    buffer.push_back('\0');

    return buffer.data();
}

auto push_id(string_view str_id) -> void
{
    const char* begin = str_id.data();
    const char* end = begin + str_id.size();
    ImGui::PushID(begin, end);
}

auto push_id(const void* ptr_id) -> void
{
    ImGui::PushID(ptr_id);
}

auto push_id(u32 id) -> void
{
    ImGui::PushID(static_cast<int>(id));
}

auto pop_id() -> void
{
    ImGui::PopID();
}

auto begin(string_view name, window_flags flags) -> bool
{
    return ImGui::Begin(imgui_cstr(name), nullptr, static_cast<ImGuiWindowFlags>(flags));
}

auto begin(string_view name, bool& is_open, window_flags flags) -> bool
{
    return ImGui::Begin(imgui_cstr(name), &is_open, static_cast<ImGuiWindowFlags>(flags));
}

auto end() -> void
{
    ImGui::End();
}

auto vtext(string_view fmt, format_args args) -> void
{
    auto formatted = imgui_format_cstr(fmt, args);
    ImGui::TextUnformatted(formatted.data(), formatted.data() + formatted.size());
}

auto selectable(string_view label, bool is_selected, selectable_flags flags) -> bool
{
    return ImGui::Selectable(imgui_cstr(label), is_selected, static_cast<ImGuiSelectableFlags>(flags));
}

} // namespace poc::gui