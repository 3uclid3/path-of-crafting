#include <poc.item/log.hpp>

namespace poc {

auto item_logger() -> log::logger*
{
    static std::shared_ptr<log::logger> logger = log::make_logger("item");
    return logger.get();
}

} // namespace poc
