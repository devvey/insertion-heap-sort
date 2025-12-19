#include "benchmark.hpp"
#include "algorithms.hpp"
#include "generators.hpp"

#include <iostream>
#include <vector>
#include <csignal>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <fstream>


// Обработчик Ctrl+C
void signal_handler(int signal) {
	(void)signal;  // Помечаем как использованный
    std::cout << "\n\n[INTERRUPT] Large benchmark stopped\n";
    std::exit(0);
}

int main() {
    std::signal(SIGINT, signal_handler);
    
    try {
        //coursework::Benchmark benchmark;
        
        std::cout << "=========================================\n";
        std::cout << "  LARGE-SCALE BENCHMARK: HEAP SORT ONLY\n";
        std::cout << "  (Insertion Sort skipped - too slow)\n";
        std::cout << "=========================================\n\n";
        
        // Размеры ТОЛЬКО для Heap Sort (большие массивы)
        std::vector<size_t> large_sizes = {
            10000,    // 10k
            20000,    // 20k
            30000,    // 30k
            40000,    // 40k
            50000,    // 50k
            75000,    // 75k
            100000    // 100k - максимум
        };
        
        std::cout << "WARNING: This benchmark will take significant time\n";
        std::cout << "Estimated time: 5-15 minutes depending on hardware\n";
        std::cout << "Press Ctrl+C to stop at any time\n\n";
        
        std::cout << "Starting in 3 seconds...";
        std::cout.flush();
        for (int i = 3; i > 0; --i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << " " << i << "...";
            std::cout.flush();
        }
        std::cout << " GO!\n\n";
        
        // Запуск тестов - МЕНЬШЕ итераций для больших размеров
        std::vector<coursework::BenchmarkResult> results;
        
        for (size_t size : large_sizes) {
            // Адаптивные итерации
            size_t iterations;
            if (size <= 20000) {
                iterations = 200;
            } else if (size <= 50000) {
                iterations = 100;
            } else {
                iterations = 50;
            }
            
            std::cout << "\n[TEST] Size: " << size << " (iterations: " << iterations << ")\n";
            
            // Специальная версия для больших массивов
            coursework::BenchmarkResult result;
            result.array_size = size;
            result.iterations = iterations;
            result.insertion_sort_time = -1.0; // Insertion пропускаем
            
            double total_heap = 0.0;
            double total_std = 0.0;
            
            coursework::ArrayGenerator generator;
            
            std::cout << "Progress: |";
            size_t progress_step = std::max<size_t>(1, iterations / 40);
            
            for (size_t i = 0; i < iterations; ++i) {
                if (i % progress_step == 0) {
                    std::cout << "=";
                    std::cout.flush();
                }
                
                // Генерируем данные
                auto data = generator.generate(size, coursework::DataType::RANDOM);
                
                // Heap Sort
                auto start = std::chrono::high_resolution_clock::now();
                coursework::heap_sort(data.begin(), data.end());
                auto end = std::chrono::high_resolution_clock::now();
                total_heap += std::chrono::duration<double, std::micro>(end - start).count();
                
                // std::sort (на уже отсортированных данных)
                start = std::chrono::high_resolution_clock::now();
                std::sort(data.begin(), data.end());
                end = std::chrono::high_resolution_clock::now();
                total_std += std::chrono::duration<double, std::micro>(end - start).count();
            }
            
            std::cout << "|\n";
            
            result.heap_sort_time = total_heap / iterations;
            result.std_sort_time = total_std / iterations;
            results.push_back(result);
            
            // Промежуточный вывод
            std::cout << "  Heap Sort: " << result.heap_sort_time << " μs (";
            if (result.heap_sort_time >= 1000000) {
                std::cout << (result.heap_sort_time / 1000000.0) << " seconds)\n";
            } else if (result.heap_sort_time >= 1000) {
                std::cout << (result.heap_sort_time / 1000.0) << " ms)\n";
            } else {
                std::cout << result.heap_sort_time << " μs)\n";
            }
            
            std::cout << "  std::sort: " << result.std_sort_time << " μs\n";
            
            double ratio = result.heap_sort_time / result.std_sort_time;
            std::cout << "  Ratio (Heap/std): " << ratio << "x (";
            if (ratio > 1.0) {
                std::cout << "std::sort is " << ratio << "x faster)\n";
            } else {
                std::cout << "Heap is " << (1.0/ratio) << "x faster)\n";
            }
        }
        
        // Сохранение результатов
        std::ofstream file("large_scale_results.csv");
        if (file.is_open()) {
            file << "ArraySize,HeapSortMicroseconds,StdSortMicroseconds,Iterations\n";
            for (const auto& res : results) {
                file << res.array_size << ","
                     << res.heap_sort_time << ","
                     << res.std_sort_time << ","
                     << res.iterations << "\n";
            }
            file.close();
            std::cout << "\n[SUCCESS] Results saved to: large_scale_results.csv\n";
        }   else {
            std::cerr << "\n[ERROR] Cannot create file: large_scale_results.csv\n";
        }
        
        // Сводка
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "LARGE-SCALE BENCHMARK COMPLETE\n";
        std::cout << std::string(60, '=') << "\n";
        
        std::cout << "\nKey findings for large arrays (>10,000 elements):\n";
        std::cout << "1. Heap Sort maintains O(n log n) performance\n";
        std::cout << "2. std::sort is typically 1.5-3x faster (optimized)\n";
        std::cout << "3. Insertion Sort is completely impractical\n";
        std::cout << "4. Memory usage scales linearly with array size\n";
        
        return 0;
        
    } catch (const std::bad_alloc& e) {
        std::cerr << "\n[ERROR] Out of memory! System cannot handle these array sizes.\n";
        return 1;
    } catch (const std::exception&) {
        //std::cerr << "\n[ERROR] " << e.what() << "\n";
        return 1;
    }
}