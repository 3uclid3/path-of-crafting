#include <poc.platform/log.hpp>

#include <poc.core/def.hpp>

namespace poc::platform {

POC_COVERAGE_DISABLED auto logger() -> log::logger*
{
    static const auto logger = log::make_logger("platform");
    return logger.get();
}

} // namespace poc::platform