#pragma once

#include <poc.item_detail/drawer.hpp>
#include <poc.workspace/extension.hpp>

namespace poc::item_detail {

class extension final : public workspace::extension
{
public:
    static constexpr id_type static_id{workspace::make_extension_id("poc.item_detail")};

    auto id() const -> id_type override;

    auto init(init_context& context) -> init_result override;
    auto update(update_context& context) -> void override;

private:
    drawer _drawer;
};

} // namespace poc::item_detail
