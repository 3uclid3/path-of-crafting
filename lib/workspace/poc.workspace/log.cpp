#include <poc.workspace/log.hpp>

namespace poc::workspace {

auto logger() -> log::logger*
{
    static const std::shared_ptr<log::logger> logger = log::make_logger("workspace");
    return logger.get();
}

} // namespace poc::workspace