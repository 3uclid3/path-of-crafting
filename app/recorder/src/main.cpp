#include <poc/return_true.hpp>

auto main() -> int
{
    return poc::return_true() ? 0 : 1;
}
