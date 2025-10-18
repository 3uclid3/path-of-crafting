#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>

#include <poc.item/item.hpp>
#include <poc.item/item_blob.hpp>
#include <poc.item/item_parser.hpp>

namespace poc::workspace {

class ingester
{
public:
    ingester();
    ~ingester() noexcept;

    ingester(const ingester&) = delete;
    ingester& operator=(const ingester&) = delete;

    [[nodiscard]] auto can_submit(string_view raw) const noexcept -> bool;

    auto submit(item_blob&& blob) -> void;
    auto drain(std::vector<item>& out) -> std::size_t;

private:
    auto run() -> void;

private:
    bool _running{true};
    std::thread _thread;

    std::mutex _mutex;
    std::condition_variable _cv;

    std::deque<item_blob> _blobs;
    std::vector<item> _out;

    item_parser _parser;
};

} // namespace poc::workspace
