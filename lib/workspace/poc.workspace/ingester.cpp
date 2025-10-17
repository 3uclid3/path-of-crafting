#include <poc.workspace/ingester.hpp>

#include <poc.workspace/log.hpp>

namespace poc::workspace {

ingester::ingester()
{
    _out.reserve(32);

    _thread = std::thread(&ingester::run, this);
}

ingester::~ingester() noexcept
{
    _running.store(false, std::memory_order_relaxed);

    _cv.notify_all();

    if (_thread.joinable())
    {
        _thread.join();
    }
}

auto ingester::can_submit(string_view raw) const noexcept -> bool
{
    return _parser.can_parse(raw);
}

auto ingester::submit(item_blob&& blob) -> void
{
    POC_WORKSPACE_TRACE("Submitting blob:\n{}", blob);

    {
        std::lock_guard lock(_mutex);

        _blobs.emplace_back(std::move(blob));
    }

    _cv.notify_one();
}

auto ingester::drain(std::vector<item>& out) -> std::size_t
{
    std::lock_guard lock(_mutex);

    const std::size_t size = _out.size();
    out.insert(out.end(), std::make_move_iterator(_out.begin()), std::make_move_iterator(_out.end()));
    _out.clear();

    return size;
}

auto ingester::run() -> void
{
    std::deque<item_blob> local_blobs;

    std::vector<item> parsed_items;
    parsed_items.reserve(_out.capacity());

    while (_running.load(std::memory_order_relaxed))
    {
        {
            std::unique_lock lock(_mutex);

            _cv.wait(lock, [this]() {
                return !_blobs.empty() || !_running.load(std::memory_order_relaxed);
            });

            std::swap(local_blobs, _blobs);
        }

        while (!local_blobs.empty())
        {
            if (auto item = _parser.parse(std::move(local_blobs.front())); item.has_value())
            {
                parsed_items.emplace_back(std::move(item.value()));
            }

            local_blobs.pop_front();
        }

        if (!parsed_items.empty())
        {
            std::lock_guard guard(_mutex);

            _out.insert(_out.end(), std::make_move_iterator(parsed_items.begin()), std::make_move_iterator(parsed_items.end()));

            parsed_items.clear();
        }
    }
}

} // namespace poc::workspace
