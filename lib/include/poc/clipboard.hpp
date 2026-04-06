#pragma once

#include <functional>
#include <string_view>

namespace poc::clipboard {

using changed_callback = std::function<void(std::string_view)>;

auto set_changed_callback(changed_callback callback) -> void;

} // namespace poc::clipboard
