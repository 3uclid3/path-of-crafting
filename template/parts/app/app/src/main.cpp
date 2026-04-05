#include <xct/return_true.hpp>

auto main() -> int
{
    return xct::return_true() ? 0 : 1;
}
