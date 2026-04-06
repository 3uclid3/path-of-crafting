#define NOMINMAX
#include <windows.h>

#include <atomic>
#include <cassert>
#include <chrono>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include <argparse/argparse.hpp>

#include <poc/clipboard.hpp>
#include <poc/item.hpp>

namespace poc {

using clock = std::chrono::system_clock;

class recorder;

namespace detail {

} // namespace detail

class recorder
{
public:
    inline static const auto save_interval = std::chrono::minutes(5);

public:
    recorder(std::string&& output_file, bool verbose)
        : _output_file(std::move(output_file))
        , _stream(_output_file, std::ios::binary)
        , _verbose(verbose)
    {
        init_instance();
        init_exit_handler();
        init_clipboard_watcher();

        _stream << _session_start_time << "\n\n";
    }

    auto run() -> void
    {
        std::cout << "[*] Recorder started. Output file: " << _output_file << "\n";

        while (_running.load())
        {
            poll_events();

            maybe_flush();

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        _stream.flush();
    }

private:
    auto init_instance() -> void
    {
        assert(_instance == nullptr && "Only one instance of recorder is allowed");
        _instance = this;
    }

    auto init_exit_handler() -> void
    {
        ::SetConsoleCtrlHandler([]([[maybe_unused]] DWORD ctrl_type) { _instance->_running.store(false); return TRUE; }, TRUE);
    }

    auto init_clipboard_watcher() -> void
    {
        clipboard::set_changed_callback([this](std::string_view blob) { on_clipboard_changed(blob); });
    }

    auto poll_events() -> void
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
    }

    auto maybe_flush() -> void
    {
        clock::time_point now = clock::now();
        clock::duration elapsed = now - _last_flush_time;

        if (elapsed >= save_interval)
        {
            _stream.flush();
            _last_flush_time = now;
        }
    }

    auto on_clipboard_changed(std::string_view blob) -> void
    {
        if (item::is_magic_blob(blob))
        {
            if (_verbose)
            {
                std::cout << "[*] Magic item: " << item::parse_name(blob) << "\n";
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

    inline static recorder* _instance{nullptr};
};

} // namespace poc

auto main(int argc, char* argv[]) -> int
{
    argparse::ArgumentParser cli{"Path of Crafting - Recorder"};

    cli.add_argument("-o", "--output")
        .help("Path to the output file where recorded items will be saved.")
        .default_value(std::format("{:%Y-%m-%dT%H-%M-%S}.pocr", poc::clock::now()))
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
