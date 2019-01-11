#include <benchmark/benchmark.h>
#include <iostream>
#include <memory>
#include <cstring>
#include <chrono>
#include <fstream>
#include <immintrin.h>
#include <cblas.h>

static auto UNIT = benchmark::kMillisecond;
using Type = float;
using BenchFn = void(*)(const Type*, Type*, size_t, size_t);

#define REPETITIONS 64
#define AT(row, col, size) (row * size + col)

#define ELEMS_PER_CACHE_LINE (64 / sizeof(Type))

#define BENCH_DEFINE(Name) BENCHMARK(Name)->Unit(UNIT)

#define BENCH(Fn) static void Bench##Fn(benchmark::State& state) \
{ \
    benchmarkBody<Fn>(state); \
} \
BENCH_DEFINE(Bench##Fn)

//#define ASSERT


static void allocMatrix(Type* __restrict__ * mem, size_t size)
{
    if (posix_memalign((void**)mem, 64, size * size * sizeof(Type)))
    {
        std::cerr << "Couldn't allocate memory" << std::endl;
        exit(1);
    }
    assert(((uintptr_t)*mem & 0x3F) == 0);
    std::memset(*mem, 0, size * size * sizeof(Type));
}
static void createMatrix(Type* __restrict__ * a, Type* __restrict__ * b, size_t size)
{
    allocMatrix(a, size);
    allocMatrix(b, size);

    for (size_t i = 0; i < size * size; i++)
    {
        (*a)[i] = i % 5;
        (*b)[i] = i % 5;
    }
}
void freeMatrix(Type* ptr)
{
    free(ptr);
}

template<typename... Args>
void freeMatrix(Type* ptr, Args... args)
{
    free(ptr);
    freeMatrix(args...) ;
}
static void assertMatrix(const Type* a, const Type* b, size_t size)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (a[AT(i, j, size)] != b[AT(j, i, size)])
            {
                std::cerr << "Wrong result" << std::endl;
                std::exit(1);
            }
}
static Type* copyMatrix(Type* mat, size_t size)
{
    Type* copy;
    allocMatrix(&copy, size);
    std::memcpy(copy, mat, size * size * sizeof(Type));
    return copy;
}
static void printMatrix(const Type* mat, size_t size, std::ostream& of)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            of << mat[AT(i, j, size)] << " ";
        }
        of << std::endl;
    }
}
static void transposeMatrix(Type* mat, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = i + 1; j < size; j++)
        {
            std::swap(mat[AT(i, j, size)], mat[AT(j, i, size)]);
        }
    }
}

template <void(*FN)(const Type*, Type*, size_t, size_t)>
static void benchmarkBody(benchmark::State& state)
{
    auto size = static_cast<size_t>(state.range(0));
    auto tile = static_cast<size_t>(state.range(1));
    Type *a, *b;
    createMatrix(&a, &b, size);

#ifndef ASSERT
    for (auto _ : state)
#endif
    {
        FN(a, b, size, tile);
#ifdef ASSERT
        assertMatrix(a, b, size);
#endif
    }

    freeMatrix(a, b);
}

void TransposeSimple(const Type* __restrict__ a, Type* __restrict__ b, size_t size, size_t tileSize)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            b[AT(i, j, size)] = a[AT(j, i, size)];
        }
    }
}
void TransposeTiled(const Type* __restrict__ a, Type* __restrict__ b, size_t size, size_t tileSize)
{
    for (int i = 0; i < size; i += tileSize)
    {
        for (int j = 0; j < size; j += tileSize)
        {
            for (int ii = i; ii < i + tileSize; ii++)
            {
                for (int jj = j; jj < j + tileSize; jj++)
                {
                    b[AT(ii, jj, size)] = a[AT(jj, ii, size)];
                }
            }
        }
    }
}

BENCH(TransposeSimple)->RangeMultiplier(2)->Ranges({{1024, 8192}, {1, 1}});
BENCH(TransposeTiled)->RangeMultiplier(2)->Ranges({{1024, 8192}, {8, 128}});

BENCHMARK_MAIN();
