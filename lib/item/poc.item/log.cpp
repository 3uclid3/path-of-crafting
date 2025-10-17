#include <poc.item/log.hpp>

#include <poc.core/def.hpp>

namespace poc {

POC_COVERAGE_DISABLED auto item_logger() -> log::logger*
{
    static std::shared_ptr<log::logger> logger = log::make_logger("item");
    return logger.get();
}

} // namespace poc
