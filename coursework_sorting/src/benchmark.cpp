#include "benchmark.hpp"
#include "algorithms.hpp"
#include "generators.hpp"
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <stdexcept>
#include <sstream>  // для std::stringstream

namespace coursework {

BenchmarkResult Benchmark::run_single_test(size_t array_size, size_t iterations, DataType data_type) {
    ArrayGenerator generator;
    BenchmarkResult result;
    result.array_size = array_size;
    result.iterations = iterations;

    double total_insertion = 0.0;
    double total_heap = 0.0;
    double total_std = 0.0;

    size_t completed_iterations = 0;
    bool insertion_enabled = (array_size <= 1000);

    for (size_t i = 0; i < iterations; ++i) {
        std::vector<int> data = generator.generate(array_size, data_type);
        
        if (data.size() != array_size) {
            throw std::runtime_error("Generated data size mismatch");
        }

        // Insertion Sort
        if (insertion_enabled) {
            std::vector<int> data1 = data;
            auto start = std::chrono::high_resolution_clock::now();
            insertion_sort(data1.begin(), data1.end());
            auto end = std::chrono::high_resolution_clock::now();
            total_insertion += std::chrono::duration<double, std::micro>(end - start).count();
            
            // Проверка сортировки
            if (!std::is_sorted(data1.begin(), data1.end())) {
                throw std::runtime_error("Insertion sort failed");
            }
        }

        // Heap Sort
        std::vector<int> data2 = data;
        auto start = std::chrono::high_resolution_clock::now();
        heap_sort(data2.begin(), data2.end());
        auto end = std::chrono::high_resolution_clock::now();
        total_heap += std::chrono::duration<double, std::micro>(end - start).count();
        
        // Проверка сортировки
        if (!std::is_sorted(data2.begin(), data2.end())) {
            throw std::runtime_error("Heap sort failed");
        }

        // std::sort
        std::vector<int> data3 = data;
        start = std::chrono::high_resolution_clock::now();
        std::sort(data3.begin(), data3.end());
        end = std::chrono::high_resolution_clock::now();
        total_std += std::chrono::duration<double, std::micro>(end - start).count();

        completed_iterations++;
    }

    if (completed_iterations > 0) {
        result.insertion_sort_time = insertion_enabled ? total_insertion / completed_iterations : -1.0;
        result.heap_sort_time = total_heap / completed_iterations;
        result.std_sort_time = total_std / completed_iterations;
    }

    return result;
}

std::vector<BenchmarkResult> Benchmark::run_test_suite(const std::vector<size_t>& sizes, size_t iterations, DataType data_type) {
    std::vector<BenchmarkResult> results;
    std::cout << "\n=== BENCHMARK SUITE START ===\n";

    for (size_t size : sizes) {
        size_t actual_iterations = iterations;
        if (size > 5000) actual_iterations = std::min(iterations, static_cast<size_t>(50));
        else if (size > 2000) actual_iterations = std::min(iterations, static_cast<size_t>(100));

        std::cout << "Testing size: " << size << " (iterations: " << actual_iterations << ")\n";
        
        auto result = run_single_test(size, actual_iterations, data_type);
        result.iterations = actual_iterations;
        results.push_back(result);
    }

    std::cout << "=== BENCHMARK SUITE COMPLETED ===\n";
    return results;
}

// Вспомогательная функция для форматирования времени
std::string format_time(double microseconds) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);
    
    if (microseconds < 0) {
        return "N/A";
    } else if (microseconds < 1000) {
        ss << microseconds << " us";
    } else if (microseconds < 1000000) {
        ss << (microseconds / 1000.0) << " ms";
    } else {
        ss << (microseconds / 1000000.0) << " s";
    }
    return ss.str();
}

void Benchmark::print_results(const std::vector<BenchmarkResult>& results) {
    if (results.empty()) {
        std::cout << "No results.\n";
        return;
    }

    // Устанавливаем локаль для корректного отображения символов
    #ifdef _WIN32
        system("chcp 65001 > nul");  // UTF-8 в Windows
    #endif

    std::cout << std::string(80, '=') << "\n";
    std::cout << "RESULTS (average time)\n";
    std::cout << std::string(80, '=') << "\n";
    std::cout << std::left << std::setw(10) << "Size"
              << std::setw(25) << "Insertion Sort"
              << std::setw(25) << "Heap Sort"
              << std::setw(25) << "std::sort" << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (const auto& res : results) {
        std::string insertion = res.insertion_sort_time >= 0 ? 
            format_time(res.insertion_sort_time) : "skipped";
        
        std::cout << std::left << std::setw(10) << res.array_size
                  << std::setw(25) << insertion
                  << std::setw(25) << format_time(res.heap_sort_time)
                  << std::setw(25) << format_time(res.std_sort_time) << "\n";
    }
    std::cout << std::string(80, '=') << "\n";
    
    // Добавляем таблицу сравнения производительности
    std::cout << "\nPERFORMANCE COMPARISON (Heap vs Insertion):\n";
    std::cout << std::string(60, '-') << "\n";
    std::cout << std::left << std::setw(10) << "Size"
              << std::setw(20) << "Insertion/Heap"
              << std::setw(20) << "Heap/std::sort" << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const auto& res : results) {
        if (res.insertion_sort_time > 0 && res.heap_sort_time > 0) {
            double insertion_vs_heap = res.insertion_sort_time / res.heap_sort_time;
            double heap_vs_std = res.heap_sort_time / res.std_sort_time;
            
            std::stringstream ss1, ss2;
            ss1 << std::fixed << std::setprecision(2) << insertion_vs_heap << "x";
            ss2 << std::fixed << std::setprecision(2) << heap_vs_std << "x";
            
            std::cout << std::left << std::setw(10) << res.array_size
                      << std::setw(20) << ss1.str()
                      << std::setw(20) << ss2.str() << "\n";
        }
    }
    std::cout << std::string(60, '=') << "\n";
}

void Benchmark::save_to_csv(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: cannot create file " << filename << "\n";
        return;
    }

    file << "Size,InsertionSort(us),HeapSort(us),StdSort(us),Iterations\n";
    for (const auto& res : results) {
        file << res.array_size << "," 
             << res.insertion_sort_time << "," 
             << res.heap_sort_time << "," 
             << res.std_sort_time << "," 
             << res.iterations << "\n";
    }
    file.close();
    std::cout << "Results saved to " << filename << "\n";
}

} // namespace coursework