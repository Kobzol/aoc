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
using BenchFn = void(*)(const Type*, const Type*, Type*, size_t);

#define REPETITIONS 64
#define AT(row, col, size) (row * size + col)

#define ELEMS_PER_CACHE_LINE (64 / sizeof(Type))

#define BENCH_DEFINE(Name) BENCHMARK(Name)->Unit(UNIT)->RangeMultiplier(2)->Range(1024, 1024)

#define BENCH(Fn, transpose) static void Bench##Fn(benchmark::State& state) \
{ \
    benchmarkBody<transpose, Fn>(state); \
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
static void createMatrix(Type* __restrict__ * a, Type* __restrict__ * b, Type* __restrict__  * c, size_t size)
{
    allocMatrix(a, size);
    allocMatrix(b, size);
    allocMatrix(c, size);

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
static void assertMatrix(const Type* a, const Type* b, const Type* c, size_t size)
{
    auto res = std::unique_ptr<Type[]>(new Type[size * size]());
    for (int i = 0; i < size; i++)
        for (int k = 0; k < size; k++)
            for (int j = 0; j < size; j++)
            res.get()[AT(i, j, size)] += a[AT(i, k, size)] * b[AT(k, j, size)];

    for (size_t i = 0; i < size * size; i++)
    {
        if (std::abs(res[i] - c[i]) > 0.0001)
        {
            std::cerr << "Wrong result: expected " << res[i] << ", got " << c[i] << " (" << std::abs(res[i] - c[i]) << ")" << std::endl;
            std::cerr << "Index: " << i << " (" << (i / size) << ", " << (i % size) << ")" << std::endl;
            std::exit(1);
        }
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

template <bool TRANSPOSE, void(*FN)(const Type*, const Type*, Type*, size_t)>
static void benchmarkBody(benchmark::State& state)
{
    auto size = static_cast<size_t>(state.range(0));
    Type *a, *b, *c;
    createMatrix(&a, &b, &c, size);

    Type* d = b;
    if (TRANSPOSE)
    {
        d = copyMatrix(b, size);
        transposeMatrix(d, size);
    }

#ifndef ASSERT
    for (auto _ : state)
#endif
    {
        FN(a, d, c, size);
#ifdef ASSERT
        assertMatrix(a, b, c, size);
#endif
    }

    if (TRANSPOSE)
    {
        freeMatrix(a, b, c, d);
    }
    else freeMatrix(a, b, c);
}

void MatMulIJK(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            for (int k = 0; k < size; k++)
                c[AT(i, j, size)] += a[AT(i, k, size)] * b[AT(k, j, size)];
}
void MatMulIKJ(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int i = 0; i < size; i++)
        for (int k = 0; k < size; k++)
            for (int j = 0; j < size; j++)
                c[AT(i, j, size)] += a[AT(i, k, size)] * b[AT(k, j, size)];
}
void MatMulKIJ(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int k = 0; k < size; k++)
        for (int i = 0; i < size; i++)
            for (int j = 0; j < size; j++)
                c[AT(i, j, size)] += a[AT(i, k, size)] * b[AT(k, j, size)];
}
void MatMulKJI(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int k = 0; k < size; k++)
        for (int j = 0; j < size; j++)
            for (int i = 0; i < size; i++)
                c[AT(i, j, size)] += a[AT(i, k, size)] * b[AT(k, j, size)];
}
void MatMulJIK(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int j = 0; j < size; j++)
        for (int i = 0; i < size; i++)
            for (int k = 0; k < size; k++)
                c[AT(i, j, size)] += a[AT(i, k, size)] * b[AT(k, j, size)];
}
void MatMulJKI(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int j = 0; j < size; j++)
        for (int k = 0; k < size; k++)
            for (int i = 0; i < size; i++)
                c[AT(i, j, size)] += a[AT(i, k, size)] * b[AT(k, j, size)];
}
void MatMulTranspose(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            auto val = 0.0f;
            for (int k = 0; k < size; k++)
            {
                val += a[AT(i, k, size)] * b[AT(j, k, size)];
            }
            c[AT(i, j, size)] = val;
        }
    }
}
void MatMulTransposeUnrolled4(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            Type val[4] = { 0.0f };
            for (int k = 0; k < size; k += 4)
            {
                val[0] += a[AT(i, k, size)] * b[AT(j, k, size)];
                val[1] += a[AT(i, k + 1, size)] * b[AT(j, k + 1, size)];
                val[2] += a[AT(i, k + 2, size)] * b[AT(j, k + 2, size)];
                val[3] += a[AT(i, k + 3, size)] * b[AT(j, k + 3, size)];
            }
            c[AT(i, j, size)] = val[0] + val[1] + val[2] + val[3];
        }
    }
}
void MatMulTransposeUnrolled8(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            Type val[8] = { 0.0f };
            for (int k = 0; k < size; k += 8)
            {
                val[0] += a[AT(i, k, size)] * b[AT(j, k, size)];
                val[1] += a[AT(i, k + 1, size)] * b[AT(j, k + 1, size)];
                val[2] += a[AT(i, k + 2, size)] * b[AT(j, k + 2, size)];
                val[3] += a[AT(i, k + 3, size)] * b[AT(j, k + 3, size)];
                val[4] += a[AT(i, k + 4, size)] * b[AT(j, k + 4, size)];
                val[5] += a[AT(i, k + 5, size)] * b[AT(j, k + 5, size)];
                val[6] += a[AT(i, k + 6, size)] * b[AT(j, k + 6, size)];
                val[7] += a[AT(i, k + 7, size)] * b[AT(j, k + 7, size)];
            }
            c[AT(i, j, size)] = val[0] + val[1] + val[2] + val[3] + val[4] + val[5] + val[6] + val[7];
        }
    }
}
void MatMulBlocked(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int i = 0; i < size; i += ELEMS_PER_CACHE_LINE)
    {
        for (int j = 0; j < size; j += ELEMS_PER_CACHE_LINE)
        {
            for (int k = 0; k < size; k += ELEMS_PER_CACHE_LINE)
            {
                auto rres = c + AT(i, j, size);
                auto rmul1 = a + AT(i, k, size);
                for (int i2 = 0; i2 < ELEMS_PER_CACHE_LINE; i2++, rres += size, rmul1 += size)
                {
                    auto rmul2 = b + AT(k, j, size);
                    for (int k2 = 0; k2 < ELEMS_PER_CACHE_LINE; k2++, rmul2 += size)
                    {
                        for (int j2 = 0; j2 < ELEMS_PER_CACHE_LINE; j2++)
                        {
                            rres[j2] += rmul1[k2] * rmul2[j2];
                        }
                    }
                }
            }
        }
    }
}
void MatMulTransposeVectorised128(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    alignas(32) float data[4];

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            auto val = _mm_setzero_ps();
            for (int k = 0; k < size; k += 4)
            {
                auto vala = _mm_load_ps(a + AT(i, k, size));
                auto valb = _mm_load_ps(b + AT(j, k, size));
                auto valc = _mm_mul_ps(vala, valb);
                val = _mm_add_ps(valc, val);
            }

            _mm_store_ps(data, val);
            Type res = 0;
            for (auto value: data)
            {
                res += value;
            }
            c[AT(i, j, size)] = res;
        }
    }
}
void MatMulTransposeVectorised256(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    alignas(32) float data[8];

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            auto val = _mm256_setzero_ps();
            for (int k = 0; k < size; k += 8)
            {
                auto vala = _mm256_load_ps(a + AT(i, k, size));
                auto valb = _mm256_load_ps(b + AT(j, k, size));
                auto valc = _mm256_mul_ps(vala, valb);
                val = _mm256_add_ps(valc, val);
            }

            _mm256_store_ps(data, val);
            Type res = 0;
            for (auto value: data)
            {
                res += value;
            }
            c[AT(i, j, size)] = res;
        }
    }
}
void MatMulTransposeVectorised256FMA(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    alignas(32) float data[8];

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            auto val = _mm256_setzero_ps();
            for (int k = 0; k < size; k += 8)
            {
                auto vala = _mm256_load_ps(a + AT(i, k, size));
                auto valb = _mm256_load_ps(b + AT(j, k, size));
                val = _mm256_fmadd_ps(vala, valb, val);
            }

            _mm256_store_ps(data, val);
            Type res = 0;
            for (auto value: data)
            {
                res += value;
            }
            c[AT(i, j, size)] = res;
        }
    }
}
void MatMulTransposeVectorised256Unrolled4(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            __m256 vals[4];
            for (int x = 0; x < 4; x++) vals[x] = _mm256_setzero_ps();

            for (int k = 0; k < size; k += 32)
            {
                vals[0] = _mm256_add_ps(_mm256_mul_ps(_mm256_load_ps(a + AT(i, k, size)), _mm256_load_ps(b + AT(j, k, size))), vals[0]);
                vals[1] = _mm256_add_ps(_mm256_mul_ps(_mm256_load_ps(a + AT(i, k + 8, size)), _mm256_load_ps(b + AT(j, k + 8, size))), vals[1]);
                vals[2] = _mm256_add_ps(_mm256_mul_ps(_mm256_load_ps(a + AT(i, k + 16, size)), _mm256_load_ps(b + AT(j, k + 16, size))), vals[2]);
                vals[3] = _mm256_add_ps(_mm256_mul_ps(_mm256_load_ps(a + AT(i, k + 24, size)), _mm256_load_ps(b + AT(j, k + 24, size))), vals[3]);
            }

            Type res = 0;
            alignas(64) Type val[8];
            for (int x = 0; x < 4; x++)
            {
                _mm256_store_ps(val, vals[x]);
                for (auto value: val)
                {
                    res += value;
                }
            }

            c[AT(i, j, size)] = res;
        }
    }
}
void MatMulTransposeVectorised256Unrolled4FMA(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            __m256 vals[4];
            for (int x = 0; x < 4; x++) vals[x] = _mm256_setzero_ps();

            for (int k = 0; k < size; k += 32)
            {
                vals[0] = _mm256_fmadd_ps(_mm256_load_ps(a + AT(i, k, size)), _mm256_load_ps(b + AT(j, k, size)), vals[0]);
                vals[1] = _mm256_fmadd_ps(_mm256_load_ps(a + AT(i, k + 8, size)), _mm256_load_ps(b + AT(j, k + 8, size)), vals[1]);
                vals[2] = _mm256_fmadd_ps(_mm256_load_ps(a + AT(i, k + 16, size)), _mm256_load_ps(b + AT(j, k + 16, size)), vals[2]);
                vals[3] = _mm256_fmadd_ps(_mm256_load_ps(a + AT(i, k + 24, size)), _mm256_load_ps(b + AT(j, k + 24, size)), vals[3]);
            }

            Type res = 0;
            alignas(64) Type val[8];
            for (int x = 0; x < 4; x++)
            {
                _mm256_store_ps(val, vals[x]);
                for (auto value: val)
                {
                    res += value;
                }
            }

            c[AT(i, j, size)] = res;
        }
    }
}
void MatMulBlockedVectorised256Unrolled2FMA(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
    for (int i = 0; i < size; i += ELEMS_PER_CACHE_LINE)
    {
        for (int j = 0; j < size; j += ELEMS_PER_CACHE_LINE)
        {
            for (int k = 0; k < size; k += ELEMS_PER_CACHE_LINE)
            {
                auto rres = c + AT(i, j, size);
                auto rmul1 = a + AT(i, k, size);
                for (int i2 = 0; i2 < ELEMS_PER_CACHE_LINE; i2++, rres += size, rmul1 += size)
                {
                    auto rmul2 = b + AT(k, j, size);
                    for (int k2 = 0; k2 < ELEMS_PER_CACHE_LINE; k2++, rmul2 += size)
                    {
                        auto veca = _mm256_set1_ps(rmul1[k2]);
                        auto vecb = _mm256_load_ps(rmul2);
                        auto vecc = _mm256_load_ps(rres);
                        _mm256_store_ps(rres, _mm256_fmadd_ps(veca, vecb, vecc));
                        auto vecb2 = _mm256_load_ps(rmul2 + 8);
                        auto vecc2 = _mm256_load_ps(rres + 8);
                        _mm256_store_ps(rres + 8, _mm256_fmadd_ps(veca, vecb2, vecc2));
                    }
                }
            }
        }
    }
}
void MatMulBlockedVectorised256Unrolled2FMAThreaded(const Type* __restrict__ a, const Type* __restrict__ b, Type* __restrict__ c, size_t size)
{
#pragma omp parallel for
    for (int i = 0; i < size; i += ELEMS_PER_CACHE_LINE)
    {
        for (int j = 0; j < size; j += ELEMS_PER_CACHE_LINE)
        {
            for (int k = 0; k < size; k += ELEMS_PER_CACHE_LINE)
            {
                auto rres = c + AT(i, j, size);
                auto rmul1 = a + AT(i, k, size);
                for (int i2 = 0; i2 < ELEMS_PER_CACHE_LINE; i2++, rres += size, rmul1 += size)
                {
                    auto rmul2 = b + AT(k, j, size);
                    for (int k2 = 0; k2 < ELEMS_PER_CACHE_LINE; k2++, rmul2 += size)
                    {
                        auto veca = _mm256_set1_ps(rmul1[k2]);
                        auto vecb = _mm256_load_ps(rmul2);
                        auto vecc = _mm256_load_ps(rres);
                        _mm256_store_ps(rres, _mm256_fmadd_ps(veca, vecb, vecc));
                        auto vecb2 = _mm256_load_ps(rmul2 + 8);
                        auto vecc2 = _mm256_load_ps(rres + 8);
                        _mm256_store_ps(rres + 8, _mm256_fmadd_ps(veca, vecb2, vecc2));
                    }
                }
            }
        }
    }
}

void BenchMatMulBlas(benchmark::State& state)
{
    auto size = static_cast<size_t>(state.range(0));
    Type *a, *b, *c;
    createMatrix(&a, &b, &c, size);
    for (auto _ : state)
    {
        cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, size, size, size, 1.0, a, size, b, size, 1.0, c, size);
    }
    freeMatrix(a, b, c);
}

BENCH(MatMulKJI, false);
BENCH(MatMulJKI, false);
BENCH(MatMulJIK, false);
BENCH(MatMulIJK, false);
BENCH(MatMulIKJ, false);
BENCH(MatMulKIJ, false);
BENCH(MatMulTranspose, true);
BENCH(MatMulBlocked, false);
BENCH(MatMulTransposeUnrolled4, true);
BENCH(MatMulTransposeUnrolled8, true);
BENCH(MatMulTransposeVectorised128, true);
BENCH(MatMulTransposeVectorised256, true);
BENCH(MatMulTransposeVectorised256FMA, true);
BENCH(MatMulTransposeVectorised256Unrolled4, true);
BENCH(MatMulTransposeVectorised256Unrolled4FMA, true);
BENCH(MatMulBlockedVectorised256Unrolled2FMA, false);
BENCH(MatMulBlockedVectorised256Unrolled2FMAThreaded, false);
BENCH_DEFINE(BenchMatMulBlas);

BENCHMARK_MAIN();

/*int main()
{
    size_t size = 256;
    Type *a, *b, *c;
    createMatrix(&a, &b, &c, size);

    Type* d = copyMatrix(b, size);
    transposeMatrix(d, size);

    for (int i = 0; i < REPETITIONS; i++)
    {
        MatMulTransposeVectorised256(a, d, c, size);
    }
    freeMatrix(a, b, c, d);

    return 0;
}*/
