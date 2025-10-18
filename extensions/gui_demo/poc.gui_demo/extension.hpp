#pragma once

#include <poc.workspace/extension.hpp>

namespace poc::gui_demo {

class extension final : public workspace::extension
{
public:
    static constexpr id_type static_id = "poc.gui_demo";

    auto id() const -> id_type override;

    auto init(init_context& context) -> init_result override;
    auto update(update_context& context) -> void override;
};

} // namespace poc::gui_demo
