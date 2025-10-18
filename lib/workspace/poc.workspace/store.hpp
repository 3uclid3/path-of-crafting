#pragma once

#include <poc.workspace/item_repository.hpp>

namespace poc::workspace {

class store
{
public:
    auto items() noexcept -> item_repository& { return m_repository; }
    auto items() const noexcept -> const item_repository& { return m_repository; }

private:
    item_repository m_repository{};
};

} // namespace poc::workspace
