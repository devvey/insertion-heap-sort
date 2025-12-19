#include "benchmark.hpp"
#include "svg_plotter.hpp"    // Вариант 1: svg
#include "ascii_plotter.hpp" // Вариант 3: ASCII графики
#include <iostream>
#include <vector>
#include <string>
#include <limits>

int main() {
    try {
        coursework::Benchmark benchmark;

        std::cout << "========================================\n";
        std::cout << "   COMPARISON OF SORTING ALGORITHMS    \n";
        std::cout << "========================================\n\n";

        // Тестовые размеры (уменьшим для быстрого теста)
        std::vector<size_t> sizes = {100, 200, 500, 1000};
        
        std::cout << "1. RANDOM DATA TEST\n";
        std::cout << "====================\n";
        auto random_results = benchmark.run_test_suite(sizes, 10, coursework::DataType::RANDOM);
        benchmark.print_results(random_results);
        benchmark.save_to_csv(random_results, "random_results.csv");

         // Генерация SVG графиков (без зависимостей!)
        std::cout << "\n\n2. GENERATING VISUALIZATION\n";
        std::cout << "===========================\n";
        coursework::SvgPlotter::create_all_svg_plots("random_results.csv");
		
		std::cout << "\n\n3. SORTED DATA TEST\n";
        std::cout << "====================\n";
        auto sorted_test = benchmark.run_single_test(1000, 5, coursework::DataType::SORTED);
        std::cout << "Array size: 1000 (5 iterations)\n";
        std::cout << "Insertion Sort: " << sorted_test.insertion_sort_time << " us\n";
        std::cout << "Heap Sort:      " << sorted_test.heap_sort_time << " us\n";
        std::cout << "std::sort:      " << sorted_test.std_sort_time << " us\n";

        std::cout << "\n4. REVERSED DATA TEST\n";
        std::cout << "======================\n";
        auto reversed_test = benchmark.run_single_test(1000, 5, coursework::DataType::REVERSED);
        std::cout << "Array size: 1000 (5 iterations)\n";
        std::cout << "Insertion Sort: " << reversed_test.insertion_sort_time << " us\n";
        std::cout << "Heap Sort:      " << reversed_test.heap_sort_time << " us\n";
        std::cout << "std::sort:      " << reversed_test.std_sort_time << " us\n";

        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "ALL TESTS COMPLETED SUCCESSFULLY!\n";
        std::cout << std::string(50, '=') << "\n";
        
        // Пауза
        std::cout << "\nPress Enter to exit...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        return 0;
    } 
    catch (const std::exception& e) {
        std::cerr << "\nERROR: " << e.what() << "\n";
        
        std::cout << "\nPress Enter to exit...";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        return 1;
    }
}