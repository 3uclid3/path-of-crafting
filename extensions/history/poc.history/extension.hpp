#pragma once

#include <poc.history/drawer.hpp>
#include <poc.workspace/extension.hpp>

namespace poc::history {

class extension final : public workspace::extension
{
public:
    static constexpr id_type static_id{workspace::make_extension_id("poc.item_view")};

    auto id() const -> id_type override;

    auto init(init_context& context) -> init_result override;
    auto update(update_context& context) -> void override;

private:
    drawer _drawer;
};

} // namespace poc::history
