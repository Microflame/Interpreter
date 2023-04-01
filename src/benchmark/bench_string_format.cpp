#include <benchmark/benchmark.h>

#include <string>
#include <sstream>

#include <fmt/core.h>

static std::string format_fmt(const std::string& str, int64_t i, double f) {
  return fmt::format("String: {}, Integer: {}, Float: {}", str, i, f);
}

static std::string format_stream(const std::string& str, int64_t i, double f) {
  std::stringstream ss;
  ss << "String: " << str << ", Integer: " << i << ", Float: " << f;
  return ss.str();
}

static void BM_fmt(benchmark::State& state) {
  std::string str = std::to_string(state.range(0));
  int64_t i = state.range(1);
  double f = double(state.range(2)) / 100.;
  for (auto _: state) {
    std::string res = format_fmt(str, i, f);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_fmt)->Args({123456789, 424242, -12345});


static void BM_stream(benchmark::State& state) {
  std::string str = std::to_string(state.range(0));
  int64_t i = state.range(1);
  double f = double(state.range(2)) / 100.;
  for (auto _: state) {
    std::string res = format_stream(str, i, f);
    benchmark::DoNotOptimize(res);
  }
}
BENCHMARK(BM_stream)->Args({123456789, 424242, -12345});

BENCHMARK_MAIN();
