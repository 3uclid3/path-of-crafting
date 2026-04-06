#include <atomic>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <argparse/argparse.hpp>

#include <poc/clipboard.hpp>
#include <poc/item.hpp>
#include <poc/platform.hpp>

namespace poc {

class recorder
{
public:
    using clock = std::chrono::system_clock;

    inline static const auto save_interval = std::chrono::minutes(5);

public:
    recorder(std::string&& output_file, bool verbose)
        : _output_file(std::move(output_file))
        , _stream(_output_file, std::ios::binary)
        , _verbose(verbose)
    {
        _stream << _session_start_time << "\n\n";

        platform::set_console_control_handler([this] { _running.store(false); return true; });
        clipboard::set_changed_callback([this](std::string_view blob) { on_clipboard_changed(blob); });
    }

    auto run() -> void
    {
        std::cout << "[*] Recorder started. Output file: " << _output_file << "\n";

        while (_running.load())
        {
            platform::poll_events();

            clock::time_point now = clock::now();
            clock::duration elapsed = now - _last_flush_time;

            if (elapsed >= save_interval)
            {
                _stream.flush();
                _last_flush_time = now;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        _stream.flush();
    }

private:
    auto on_clipboard_changed(std::string_view blob) -> void
    {
        if (item::is_magic_blob(blob))
        {
            if (_verbose)
            {
                std::cout << "[*] Magic item: " << item::parse_magic_name(blob) << "\n";
            }

            _stream << "=== " << clock::now() << "\n";
            _stream << blob << "\n";
        }
    }

private:
    clock::time_point _session_start_time{clock::now()};
    clock::time_point _last_flush_time{_session_start_time};

    std::string _output_file;
    std::ofstream _stream;

    bool _verbose{false};
    std::atomic<bool> _running{true};
};

} // namespace poc

auto main(int argc, char* argv[]) -> int
{
    argparse::ArgumentParser cli{"Path of Crafting - Recorder"};

    cli.add_argument("-o", "--output")
        .help("Path to the output file where recorded items will be saved.")
        .default_value(std::format("{:%Y-%m-%dT%H-%M-%S}.pocr", poc::recorder::clock::now()))
        .metavar("FILE");

    cli.add_argument("-v", "--verbose")
        .help("Enable verbose output.")
        .default_value(false)
        .implicit_value(true);

    cli.parse_args(argc, argv);

    poc::recorder recorder(cli.get("output"), cli.get<bool>("verbose"));
    recorder.run();
    return 0;
}
