#include <poc.workspace/log.hpp>

#include <poc.core/def.hpp>

namespace poc::workspace {

POC_COVERAGE_DISABLED auto logger() -> log::logger*
{
    static const std::shared_ptr<log::logger> logger = log::make_logger("workspace");
    return logger.get();
}

} // namespace poc::workspace