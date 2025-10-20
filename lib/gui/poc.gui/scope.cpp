#include <poc.gui/scope.hpp>

#include <imgui.h>

namespace poc::gui {

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

auto push_id(id_t id) -> void
{
    ImGui::PushID(id);
}

auto pop_id() -> void
{
    ImGui::PopID();
}

} // namespace poc::gui