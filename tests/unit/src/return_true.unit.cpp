#include <poc/return_true.hpp>

#include <doctest/doctest.h>

namespace poc { namespace {

TEST_CASE("return_true")
{
    CHECK(return_true());
}

}} // namespace poc