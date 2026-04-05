#include <xct.unit/doctest.hpp>

#include <xct/return_true.hpp>

namespace xct { namespace {

TEST_CASE("return_true")
{
    CHECK(return_true());
}

}} // namespace xct