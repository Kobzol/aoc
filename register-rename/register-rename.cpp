#include <iostream>
#include <vector>
#include <chrono>
#include <benchmark/benchmark.h>

#define SIZE 2048

#define BENCH(Fn) \
static void Bench##Fn(benchmark::State& state) \
{ \
    auto size = static_cast<size_t>(state.range(0)); \
    std::vector<size_t> a(size, 0); \
    std::vector<size_t> b(size, 0); \
    for (int i = 0; i < b.size(); i++) b[i] = static_cast<size_t>(i); \
    size_t sum = 0; \
    for (auto _ : state) \
    { \
        sum += Fn(a.data(), b.data(), a.size()); \
    } \
    if (a != b) throw "exc"; \
    benchmark::DoNotOptimize(sum); \
} \
BENCHMARK(Bench##Fn)->Unit(UNIT)->RangeMultiplier(2)->Range(SIZE, SIZE);

static auto UNIT = benchmark::kNanosecond;


extern "C" {
    size_t loop_basic(size_t* a, size_t* b, size_t count);
    size_t loop_unrolled(size_t* a, size_t* b, size_t count);
    size_t loop_unrolled_rax(size_t* a, size_t* b, size_t count);
};

BENCH(loop_basic);
BENCH(loop_unrolled);
BENCH(loop_unrolled_rax);

BENCHMARK_MAIN();
