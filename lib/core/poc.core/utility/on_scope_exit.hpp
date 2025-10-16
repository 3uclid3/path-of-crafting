#pragma once

#include <utility>

namespace poc {

template<typename T>
class on_scope_exit
{
public:
    constexpr on_scope_exit(T callback) noexcept;

    constexpr on_scope_exit(const on_scope_exit&) = delete;
    constexpr on_scope_exit& operator=(const on_scope_exit&) = delete;

    constexpr on_scope_exit(on_scope_exit&&) = delete;
    constexpr on_scope_exit& operator=(on_scope_exit&&) = delete;

    constexpr ~on_scope_exit();

private:
    T _callback;
};

template<typename T>
constexpr on_scope_exit<T>::on_scope_exit(T callback) noexcept
    : _callback(std::move(callback))
{
}

template<typename T>
constexpr on_scope_exit<T>::~on_scope_exit()
{
    _callback();
}

} // namespace poc
