#define NOMINMAX
#include <windows.h>

#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>
#include <thread>

#include <argparse/argparse.hpp>

#include <poc/clipboard.hpp>
#include <poc/item.hpp>

namespace poc {

using clock = std::chrono::system_clock;

class alerter
{
public:
    inline static const auto default_input_lock_duration = std::chrono::milliseconds(1000);

    alerter(std::regex regex, std::string sound_file, bool verbose)
        : _sound_file(std::move(sound_file))
        , _regex(std::move(regex))
        , _verbose(verbose)
    {
        init_instance();
        init_exit_handler();
        init_clipboard_watcher();
    }

    auto run() -> void
    {
        std::cout << "[*] Alerter started. " << "\n";

        while (_running.load())
        {
            MSG msg;
            while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
            }

            if (_locked && clock::now() - _input_lock_time >= _input_lock_duration)
            {
                unlock_input();
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

private:
    auto init_instance() -> void
    {
        assert(_instance == nullptr && "Only one instance of alerter is allowed");
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

    auto notify(std::string_view text) -> void
    {
        std::cout << "[*] Match found: " << item::parse_name(text) << "\n\n";

        if (_verbose)
        {
            std::cout << text << "\n\n";
        }

        ::PlaySound(_sound_file.c_str(), nullptr, SND_FILENAME | SND_ASYNC);

        lock_input();
    }

    auto on_clipboard_changed(std::string_view text) -> void
    {
        if (item::is_blob(text))
        {
            if (_verbose)
            {
                std::cout << "[*] " << item::parse_name(text) << "\n";
            }

            if (std::regex_search(text.begin(), text.end(), _regex))
            {
                notify(text);
            }
        }
    }

    auto lock_input() -> void
    {
        _locked = true;
        _input_lock_time = clock::now();

        _keyboard_hook = ::SetWindowsHookEx(WH_KEYBOARD_LL, [](int, WPARAM, LPARAM) -> LRESULT { return 1; }, nullptr, 0);
        _mouse_hook = ::SetWindowsHookEx(WH_MOUSE_LL, [](int, WPARAM, LPARAM) -> LRESULT { return 1; }, nullptr, 0);
    }

    auto unlock_input() -> void
    {
        ::UnhookWindowsHookEx(_keyboard_hook);
        ::UnhookWindowsHookEx(_mouse_hook);
        _keyboard_hook = {};
        _mouse_hook = {};
        _locked = false;
    }

private:
    std::string _sound_file;
    std::regex _regex;

    std::chrono::milliseconds _input_lock_duration{default_input_lock_duration};
    clock::time_point _input_lock_time;

    HHOOK _keyboard_hook{};
    HHOOK _mouse_hook{};

    std::atomic<bool> _running{true};

    bool _locked{false};
    bool _verbose{false};

    inline static alerter* _instance{nullptr};
};

} // namespace poc

auto main(int argc, char* argv[]) -> int
{
    argparse::ArgumentParser cli{"Path of Crafting - Alerter"};

    cli.add_argument("regex")
        .help("Regular expression to match clipboard text against.")
        .required();

    cli.add_argument("-ld", "--lock_duration")
        .help("Duration (in milliseconds) to lock input after a match is found.")
        .default_value(std::to_string(poc::alerter::default_input_lock_duration.count()));

    cli.add_argument("-sf", "--sound_file")
        .help("Path to the sound file to play when a match is found.")
        .default_value("alert.wav");

    cli.add_argument("-v", "--verbose")
        .help("Enable verbose output.")
        .default_value(false)
        .implicit_value(true);

    cli.parse_args(argc, argv);

    poc::alerter alerter(std::regex(cli.get<std::string>("regex")), cli.get<std::string>("sound_file"), cli.get<bool>("verbose"));
    alerter.run();
    return 0;
}
