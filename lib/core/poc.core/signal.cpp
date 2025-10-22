#include <poc.core/signal.hpp>

namespace poc {

any_signal_connection::any_signal_connection(any_signal_connection&& other) noexcept
    : _vtable(other._vtable)
{
    if (_vtable != nullptr)
    {
        _vtable->move(_storage.data(), other._storage.data());

        other._vtable->destroy(other._storage.data());
        other._vtable = nullptr;
    }
}

any_signal_connection& any_signal_connection::operator=(any_signal_connection&& other) noexcept
{
    if (this != &other)
    {
        if (_vtable != nullptr)
        {
            _vtable->destroy(_storage.data());
        }

        _vtable = other._vtable;

        if (_vtable != nullptr)
        {
            _vtable->move(_storage.data(), other._storage.data());

            other._vtable->destroy(other._storage.data());
            other._vtable = nullptr;
        }
    }
    return *this;
}

any_signal_connection::~any_signal_connection()
{
    if (_vtable != nullptr)
    {
        _vtable->destroy(_storage.data());
    }
}

} // namespace poc