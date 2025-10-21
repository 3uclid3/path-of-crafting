#include <doctest/doctest.h>

#include <poc.core/utility/crc32.hpp>

namespace poc { namespace {

static_assert(compile_crc32("hello") == 0x3610a686u);
static_assert(compile_crc32("world") == 0x3A771143u);
static_assert("hello"_crc32 == compile_crc32("hello"));
static_assert("world"_crc32 == compile_crc32("world"));

TEST_CASE("crc32")
{
    CHECK_EQ(crc32("hello"), 0x3610a686u);
    CHECK_EQ(crc32("world"), 0x3A771143u);
    CHECK_EQ(crc32("hello"), "hello"_crc32);
    CHECK_EQ(crc32("world"), "world"_crc32);
}

}} // namespace poc
