#pragma once
#include <vector>
#include <string>
#include <cstddef>

namespace coursework {

enum class DataType {
    RANDOM,
    SORTED,
    REVERSED,
    ALMOST_SORTED
};

struct BenchmarkResult {
    size_t array_size = 0;
    size_t iterations = 0;
    double insertion_sort_time = -1.0;
    double heap_sort_time = -1.0;
    double std_sort_time = -1.0;
};

class Benchmark {
public:
    BenchmarkResult run_single_test(size_t array_size, size_t iterations, DataType data_type);
    std::vector<BenchmarkResult> run_test_suite(const std::vector<size_t>& sizes, size_t iterations, DataType data_type);
    void print_results(const std::vector<BenchmarkResult>& results);
    void save_to_csv(const std::vector<BenchmarkResult>& results, const std::string& filename);
};

} // namespace coursework