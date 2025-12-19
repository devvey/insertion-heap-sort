// ascii_plotter.hpp
#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <limits>

namespace coursework {

class AsciiPlotter {
private:
    static const int PLOT_WIDTH = 70;
    static const int PLOT_HEIGHT = 20;
    static const int MAX_POINTS = 50; // Максимальное количество точек для отображения
    
    static void normalize_data(const std::vector<double>& data, 
                               std::vector<int>& normalized, 
                               bool invert = false) {
        normalized.clear();
        if (data.empty()) return;
        
        // Находим минимум и максимум (игнорируя отрицательные значения, которые означают N/A)
        double max_val = std::numeric_limits<double>::lowest();
        double min_val = std::numeric_limits<double>::max();
        
        for (double val : data) {
            if (val > 0) { // Только положительные значения (отрицательные - это N/A)
                if (val > max_val) max_val = val;
                if (val < min_val) min_val = val;
            }
        }
        
        // Если все значения N/A или одинаковые
        if (max_val <= std::numeric_limits<double>::lowest() || max_val == min_val) {
            normalized.assign(data.size(), PLOT_HEIGHT / 2);
            return;
        }
        
        // Нормализуем значения в диапазон [0, PLOT_HEIGHT-1]
        for (double val : data) {
            if (val > 0) {
                int norm = static_cast<int>((val - min_val) / (max_val - min_val) * (PLOT_HEIGHT - 1));
                if (invert) {
                    norm = PLOT_HEIGHT - 1 - norm; // Инвертируем для графиков времени (больше = выше)
                }
                normalized.push_back(norm);
            } else {
                normalized.push_back(-1); // Помечаем N/A
            }
        }
    }
    
    static char get_plot_char(bool has_insertion, bool has_heap, bool has_std) {
        int count = (has_insertion ? 1 : 0) + (has_heap ? 1 : 0) + (has_std ? 1 : 0);
        
        if (count == 0) return ' ';
        if (count == 1) {
            if (has_insertion) return 'I';
            if (has_heap) return 'H';
            return 'S';
        }
        if (count == 2) {
            if (has_insertion && has_heap) return 'X';
            if (has_insertion && has_std) return '/';
            return '\\';
        }
        return '*'; // Все три алгоритма
    }

public:
    static void print_ascii_plot(const std::string& csv_filename) {
        std::ifstream file(csv_filename);
        if (!file.is_open()) {
            std::cerr << "Cannot open CSV file for ASCII plot: " << csv_filename << "\n";
            return;
        }
        
        std::string line;
        std::getline(file, line); // Пропускаем заголовок
        
        std::vector<double> sizes;
        std::vector<double> insertion_times;
        std::vector<double> heap_times;
        std::vector<double> std_times;
        
        // Читаем данные
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string value;
            
            // Разбираем строку CSV
            std::vector<std::string> tokens;
            while (std::getline(ss, value, ',')) {
                tokens.push_back(value);
            }
            
            if (tokens.size() >= 4) {
                try {
                    sizes.push_back(std::stod(tokens[0]));
                    insertion_times.push_back(std::stod(tokens[1]));
                    heap_times.push_back(std::stod(tokens[2]));
                    std_times.push_back(std::stod(tokens[3]));
                } catch (...) {
                    std::cerr << "Warning: Failed to parse line in CSV\n";
                }
            }
        }
        
        file.close();
        
        if (sizes.empty()) {
            std::cerr << "No data found in CSV file\n";
            return;
        }
        
        // Ограничиваем количество точек для читаемости
        size_t step = std::max<size_t>(1, sizes.size() / MAX_POINTS);
        std::vector<double> sampled_sizes;
        std::vector<double> sampled_insertion;
        std::vector<double> sampled_heap;
        std::vector<double> sampled_std;
        
        for (size_t i = 0; i < sizes.size(); i += step) {
            sampled_sizes.push_back(sizes[i]);
            sampled_insertion.push_back(insertion_times[i]);
            sampled_heap.push_back(heap_times[i]);
            sampled_std.push_back(std_times[i]);
        }
        
        // Нормализуем данные для ASCII графика
        std::vector<int> norm_insertion, norm_heap, norm_std;
        normalize_data(sampled_insertion, norm_insertion, true); // invert=true для времени
        normalize_data(sampled_heap, norm_heap, true);
        normalize_data(sampled_std, norm_std, true);
        
        // ==================== ВЫВОД ASCII ГРАФИКА ====================
        
        std::cout << "\n" << std::string(PLOT_WIDTH, '=') << "\n";
        std::cout << "ASCII PERFORMANCE VISUALIZATION\n";
        std::cout << std::string(PLOT_WIDTH, '=') << "\n\n";
        
        std::cout << "LEGEND:\n";
        std::cout << "  I = Insertion Sort (red)\n";
        std::cout << "  H = Heap Sort (blue)\n";
        std::cout << "  S = std::sort (green)\n";
        std::cout << "  X = Insertion & Heap overlap\n";
        std::cout << "  / = Insertion & std::sort overlap\n";
        std::cout << "  \\ = Heap & std::sort overlap\n";
        std::cout << "  * = All three overlap\n";
        std::cout << "\nVertical axis: Time (higher = slower)\n";
        std::cout << "Horizontal axis: Array Size\n";
        std::cout << "\n" << std::string(PLOT_WIDTH, '-') << "\n";
        
        // Рисуем график
        for (int y = PLOT_HEIGHT - 1; y >= 0; y--) {
            // Масштаб по Y (левая ось)
            if (y == PLOT_HEIGHT - 1) {
                std::cout << "MAX |";
            } else if (y == 0) {
                std::cout << "MIN |";
            } else if (y == PLOT_HEIGHT / 2) {
                std::cout << "MID |";
            } else {
                std::cout << "    |";
            }
            
            // Данные
            for (size_t x = 0; x < sampled_sizes.size() && x < static_cast<size_t>(PLOT_WIDTH - 10); x++) {
                bool has_insertion = norm_insertion[x] == y && sampled_insertion[x] > 0;
                bool has_heap = norm_heap[x] == y;
                bool has_std = norm_std[x] == y;
                
                std::cout << get_plot_char(has_insertion, has_heap, has_std);
            }
            std::cout << "\n";
        }
        
        // Ось X
        std::cout << "    +";
        for (int i = 0; i < PLOT_WIDTH - 10 && i < static_cast<int>(sampled_sizes.size()); i++) {
            std::cout << "-";
        }
        std::cout << ">\n";
        
        // Подписи по оси X
        std::cout << "     ";
        if (sampled_sizes.size() > 0) {
            std::cout << std::setw(5) << std::left << static_cast<int>(sampled_sizes.front());
        }
        
        size_t mid_idx = sampled_sizes.size() / 2;
        if (mid_idx < sampled_sizes.size()) {
            int spaces_before = (PLOT_WIDTH - 20) / 2;
            for (int i = 0; i < spaces_before; i++) std::cout << " ";
            std::cout << std::setw(10) << std::left << static_cast<int>(sampled_sizes[mid_idx]);
        }
        
        if (sampled_sizes.size() > 1) {
            int spaces_remaining = PLOT_WIDTH - 25;
            for (int i = 0; i < spaces_remaining; i++) std::cout << " ";
            std::cout << std::setw(5) << std::right << static_cast<int>(sampled_sizes.back());
        }
        std::cout << "\n\n";
        
        // ==================== ТАБЛИЦА ДАННЫХ ====================
        
        std::cout << std::string(70, '=') << "\n";
        std::cout << "DATA SUMMARY TABLE\n";
        std::cout << std::string(70, '=') << "\n";
        
        // Определяем ширину колонок
        const int col1 = 12;
        const int col2 = 18;
        const int col3 = 18;
        const int col4 = 18;
        
        // Заголовок таблицы
        std::cout << std::left 
                  << std::setw(col1) << "Size" 
                  << std::setw(col2) << "Insertion (us)" 
                  << std::setw(col3) << "Heap (us)" 
                  << std::setw(col4) << "std::sort (us)"
                  << "Ratio (H/std)\n";
        
        std::cout << std::string(70, '-') << "\n";
        
        // Выводим каждую строку данных
        for (size_t i = 0; i < sizes.size(); i++) {
            std::cout << std::left << std::setw(col1) << static_cast<int>(sizes[i]);
            
            // Insertion Sort
            if (insertion_times[i] > 0) {
                std::cout << std::setw(col2) << std::fixed << std::setprecision(1) << insertion_times[i];
            } else {
                std::cout << std::setw(col2) << "N/A";
            }
            
            // Heap Sort и std::sort
            std::cout << std::setw(col3) << std::fixed << std::setprecision(1) << heap_times[i]
                      << std::setw(col4) << std::fixed << std::setprecision(1) << std_times[i];
            
            // Отношение Heap/std::sort
            if (std_times[i] > 0) {
                double ratio = heap_times[i] / std_times[i];
                std::cout << std::fixed << std::setprecision(2) << ratio << "x";
                
                if (ratio > 1.5) std::cout << " (slow)";
                else if (ratio < 0.67) std::cout << " (fast)";
                else std::cout << " (≈equal)";
            }
            
            std::cout << "\n";
            
            // Каждые 5 строк добавляем разделитель для читаемости
            if ((i + 1) % 5 == 0 && i + 1 < sizes.size()) {
                std::cout << std::string(70, '.') << "\n";
            }
        }
        
        std::cout << std::string(70, '-') << "\n";
        
        // ==================== СТАТИСТИКА ====================
        
        if (sizes.size() > 1) {
            std::cout << "\n" << std::string(70, '=') << "\n";
            std::cout << "PERFORMANCE STATISTICS\n";
            std::cout << std::string(70, '=') << "\n";
            
            // Находим точку, где Heap становится быстрее Insertion (если есть данные Insertion)
            bool has_insertion_data = false;
            for (double t : insertion_times) {
                if (t > 0) {
                    has_insertion_data = true;
                    break;
                }
            }
            
            if (has_insertion_data) {
                int crossover_point = -1;
                for (size_t i = 0; i < sizes.size(); i++) {
                    if (insertion_times[i] > 0 && heap_times[i] > 0) {
                        if (heap_times[i] < insertion_times[i]) {
                            crossover_point = static_cast<int>(sizes[i]);
                            break;
                        }
                    }
                }
                
                if (crossover_point > 0) {
                    std::cout << "Crossover point (Heap becomes faster than Insertion): n ≈ " 
                              << crossover_point << "\n";
                }
            }
            
            // Среднее ускорение Heap/std::sort
            double total_ratio = 0.0;
            int count_ratio = 0;
            for (size_t i = 0; i < sizes.size(); i++) {
                if (std_times[i] > 0) {
                    total_ratio += heap_times[i] / std_times[i];
                    count_ratio++;
                }
            }
            
            if (count_ratio > 0) {
                std::cout << "Average Heap/std::sort ratio: " 
                          << std::fixed << std::setprecision(2) << (total_ratio / count_ratio) 
                          << "x (";
                double avg = total_ratio / count_ratio;
                if (avg > 1.2) std::cout << "Heap is slower by " << std::fixed << std::setprecision(0) << ((avg-1)*100) << "%";
                else if (avg < 0.8) std::cout << "Heap is faster by " << std::fixed << std::setprecision(0) << ((1-avg)*100) << "%";
                else std::cout << "comparable performance";
                std::cout << ")\n";
            }
            
            // Самый большой и самый маленький массивы
            std::cout << "Smallest array tested: " << static_cast<int>(sizes.front()) 
                      << " elements\n";
            std::cout << "Largest array tested: " << static_cast<int>(sizes.back()) 
                      << " elements\n";
            
            // Производительность на самом большом массиве
            size_t last_idx = sizes.size() - 1;
            std::cout << "\nPerformance on largest array (n=" << static_cast<int>(sizes[last_idx]) << "):\n";
            
            if (insertion_times[last_idx] > 0) {
                std::cout << "  Insertion Sort: " << std::fixed << std::setprecision(1) 
                          << insertion_times[last_idx] << " us\n";
            }
            
            std::cout << "  Heap Sort:      " << std::fixed << std::setprecision(1) 
                      << heap_times[last_idx] << " us\n";
            std::cout << "  std::sort:      " << std::fixed << std::setprecision(1) 
                      << std_times[last_idx] << " us\n";
            
            if (insertion_times[last_idx] > 0 && heap_times[last_idx] > 0) {
                double ratio = insertion_times[last_idx] / heap_times[last_idx];
                std::cout << "  Heap is " << std::fixed << std::setprecision(1) << ratio 
                          << "x faster than Insertion\n";
            }
            
            if (heap_times[last_idx] > 0 && std_times[last_idx] > 0) {
                double ratio = heap_times[last_idx] / std_times[last_idx];
                std::cout << "  std::sort is " << std::fixed << std::setprecision(1) << ratio 
                          << "x faster than Heap\n";
            }
        }
        
        std::cout << std::string(70, '=') << "\n";
        std::cout << "ASCII visualization complete. Check CSV file for full data.\n";
        std::cout << std::string(70, '=') << "\n\n";
    }
    
    static void print_simple_plot(const std::vector<double>& sizes,
                                 const std::vector<double>& heap_times,
                                 const std::vector<double>& std_times,
                                 const std::string& title = "Performance Comparison") {
        if (sizes.size() != heap_times.size() || sizes.size() != std_times.size() || sizes.empty()) {
            std::cerr << "Invalid data for ASCII plot\n";
            return;
        }
        
        const int width = 60;
        const int height = 15;
        
        std::cout << "\n" << std::string(width, '-') << "\n";
        std::cout << title << "\n";
        std::cout << std::string(width, '-') << "\n";
        std::cout << "H = Heap Sort, S = std::sort, X = overlap\n\n";
        
        // Находим максимум для нормализации
        double max_time = 0;
        for (double t : heap_times) {
            if (t > max_time) max_time = t;
        }
        for (double t : std_times) {
            if (t > max_time) max_time = t;
        }
        
        if (max_time <= 0) {
            std::cout << "No valid time data\n";
            return;
        }
        
        // Рисуем график
        for (int y = height - 1; y >= 0; y--) {
            double threshold_low = max_time * (height - 1 - y) / height;
            double threshold_high = max_time * (height - y) / height;
            
            std::cout << std::setw(3) << static_cast<int>(max_time * (height - y) / height) << " |";
            
            for (size_t i = 0; i < sizes.size() && i < static_cast<size_t>(width - 10); i++) {
                bool has_heap = (heap_times[i] >= threshold_low && heap_times[i] < threshold_high);
                bool has_std = (std_times[i] >= threshold_low && std_times[i] < threshold_high);
                
                if (has_heap && has_std) std::cout << "X";
                else if (has_heap) std::cout << "H";
                else if (has_std) std::cout << "S";
                else std::cout << " ";
            }
            std::cout << "\n";
        }
        
        std::cout << "    +" << std::string(width - 10, '-') << "\n";
        std::cout << "     Size: " << static_cast<int>(sizes.front()) 
                  << " ... " << static_cast<int>(sizes.back()) << "\n";
        std::cout << std::string(width, '-') << "\n";
    }
};

} // namespace coursework