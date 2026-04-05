#include <benchmark/benchmark.h>

#include <xct/return_true.hpp>

namespace xct { namespace {

static auto bm_return_true(benchmark::State& state) -> void
{
    for (auto _ : state)
    {
        bool is_true = xct::return_true();
        benchmark::DoNotOptimize(empty_string);
    }
}

BENCHMARK(bm_return_true);

}} // namespace xct

BENCHMARK_MAIN();