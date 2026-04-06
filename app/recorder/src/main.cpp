#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include <poc/clipboard.hpp>
#include <poc/item.hpp>
#include <poc/platform.hpp>

namespace poc {

class recorder
{
public:
    using clock = std::chrono::system_clock;

    inline static const auto save_interval = std::chrono::minutes(5);

    struct recorded_item
    {
        explicit recorded_item(std::string&& blob)
            : timestamp(clock::now())
            , blob(std::move(blob))
        {
        }

        clock::time_point timestamp;
        std::string blob;
    };

public:
    recorder(std::string&& output_file, bool verbose)
        : _output_file(std::move(output_file))
        , _stream(_output_file, std::ios::binary)
        , _verbose(verbose)
    {
        _items.reserve(1024);

        _stream << _session_start_time << "\n\n";
    }

    auto run() -> void
    {
        std::cout << "[*] Recorder started. Output file: " << _output_file << "\n";

        platform::set_console_control_handler([this] { save_session(clock::now()); _running = false; });

        clipboard::set_changed_callback([this](std::string_view blob) { on_clipboard_changed(blob); });

        while (_running)
        {
            clock::time_point now = clock::now();

            platform::poll_events();

            if (should_save_session(now))
            {
                save_session(now);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

private:
    auto should_save_session(clock::time_point now) const noexcept -> bool
    {
        return now - _last_save_time >= save_interval;
    }

    auto save_session(clock::time_point now) -> void
    {
        if (_verbose)
        {
            std::cout << "[*] Saving session... (" << _items.size() << " items)\n";
        }

        for (const auto& item : _items)
        {
            _stream << "=== " << item.timestamp << "\n";
            _stream << item.blob << "\n";
        }

        _items.clear();
        _last_save_time = now;
    }

    auto on_clipboard_changed(std::string_view blob) -> void
    {
        if (item::is_magic_blob(blob))
        {
            if (_verbose)
            {
                std::cout << "[*] Magic item: " << item::parse_magic_name(blob) << "\n";
            }

            _items.emplace_back(std::string(blob));
        }
    }

private:
    clock::time_point _session_start_time{clock::now()};
    clock::time_point _last_save_time{_session_start_time};

    std::string _output_file;
    std::ofstream _stream;
    std::vector<recorded_item> _items;

    bool _verbose{false};
    bool _running{true};
};

} // namespace poc

auto main(int argc, char* argv[]) -> int
{
    if (argc < 2)
    {
        std::cerr << "[-] Usage: " << argv[0] << " <output_file> [-v]\n";
        return 1;
    }

    bool verbose = false;
    if (argc >= 3 && std::string_view(argv[2]) == "-v")
    {
        verbose = true;
    }

    poc::recorder recorder(argv[1], verbose);
    recorder.run();
    return 0;
}
