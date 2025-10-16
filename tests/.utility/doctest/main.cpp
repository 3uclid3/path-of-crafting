#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include <poc/log.hpp>

auto main(int argc, char** argv) -> int
{
    poc::log::options options;
    options.level = spdlog::level::off;
    options.flush_on_level = spdlog::level::off;

    poc::log::init(options);

    return doctest::Context(argc, argv).run();
}
