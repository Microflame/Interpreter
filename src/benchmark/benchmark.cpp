#include <benchmark/benchmark.h>

#include <iostream>
#include <cmath>

#include "slip/util/math.hpp"


static void BM_powi(benchmark::State& state) {
    int32_t power = state.range(0);
    for (auto _: state) {
        double res = slip::powi(10, power);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(BM_powi)->DenseRange(0, 200, 50);


double powi_native(int32_t base, int32_t power) {
    return powf64(base, power);
} 

static void BM_powi_native(benchmark::State& state) {
    int32_t power = state.range(0);
    for (auto _: state) {
        double res = powi_native(10, power);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(BM_powi_native)->DenseRange(0, 200, 50);

BENCHMARK_MAIN();
