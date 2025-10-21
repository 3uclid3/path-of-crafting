#include <poc.workspace/action_dispatcher.hpp>

namespace poc::workspace {

auto action_dispatcher::dispatch(action_context& context) -> void
{
    std::swap(_in, _out);

    for (auto& entry : _out)
    {
        entry.executor(context);
    }

    _out.clear();
}

} // namespace poc::workspace
