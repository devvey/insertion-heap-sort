// svg_plotter.cpp
#include "svg_plotter.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <string>
#include <cstddef>

namespace coursework {

// Инициализация статических констант
const int SvgPlotter::SVG_WIDTH = 1000;
const int SvgPlotter::SVG_HEIGHT = 700;
const int SvgPlotter::MARGIN = 80;
const int SvgPlotter::PLOT_WIDTH = SVG_WIDTH - 2 * MARGIN;
const int SvgPlotter::PLOT_HEIGHT = SVG_HEIGHT - 2 * MARGIN;

// Вспомогательные функции
double SvgPlotter::get_max_value(const std::vector<double>& data) {
    double max_val = 0.0;
    for (double val : data) {
        if (val > max_val) max_val = val;
    }
    return max_val;
}

std::string SvgPlotter::format_number(double num) {
    std::stringstream ss;
    if (num >= 1000000) {
        ss << std::fixed << std::setprecision(1) << (num / 1000000.0) << "s";
    } else if (num >= 1000) {
        ss << std::fixed << std::setprecision(1) << (num / 1000.0) << "ms";
    } else {
        ss << std::fixed << std::setprecision(0) << num << "μs";
    }
    return ss.str();
}

bool SvgPlotter::create_performance_svg(const std::string& csv_filename,
                                       const std::string& output_svg) {
    
    std::ifstream csv(csv_filename);
    if (!csv.is_open()) {
        std::cerr << "Error: Cannot open CSV file: " << csv_filename << "\n";
        return false;
    }
    
    // Читаем данные из CSV
    std::vector<double> sizes;
    std::vector<double> insertion_times;
    std::vector<double> heap_times;
    std::vector<double> std_times;
    
    std::string line;
    std::getline(csv, line); // Пропускаем заголовок
    
    while (std::getline(csv, line)) {
        std::stringstream ss(line);
        std::string value;
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
                // Пропускаем некорректные строки
            }
        }
    }
    
    csv.close();
    
    if (sizes.empty()) {
        std::cerr << "Error: No valid data found in CSV file\n";
        return false;
    }
    
    // Проверяем, есть ли данные Insertion Sort
    bool has_insertion_data = false;
    for (double t : insertion_times) {
        if (t > 0) {
            has_insertion_data = true;
            break;
        }
    }
    
    // Создаем SVG файл
    std::ofstream svg(output_svg);
    if (!svg.is_open()) {
        std::cerr << "Error: Cannot create SVG file: " << output_svg << "\n";
        return false;
    }
    
    // Цвета для алгоритмов
    Color colors;
    colors.insertion = "#FF4444";  // Красный
    colors.heap = "#4444FF";       // Синий
    colors.stdsort = "#44AA44";    // Зеленый
    colors.grid = "#E0E0E0";       // Серый
    colors.axis = "#000000";       // Черный
    colors.text = "#333333";       // Темно-серый
    colors.bg = "#FFFFFF";         // Белый
    
    // ==================== НАЧАЛО SVG ====================
    svg << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    svg << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    svg << "<svg width=\"" << SVG_WIDTH << "\" height=\"" << SVG_HEIGHT << "\" "
        << "xmlns=\"http://www.w3.org/2000/svg\" "
        << "xmlns:xlink=\"http://www.w3.org/1999/xlink\">\n\n";
    
    // ==================== СТИЛИ ====================
    svg << "<defs>\n";
    svg << "  <style type=\"text/css\">\n";
    svg << "    <![CDATA[\n";
    svg << "      text { font-family: Arial, sans-serif; font-size: 12px; }\n";
    svg << "      .title { font-size: 18px; font-weight: bold; }\n";
    svg << "      .axis-label { font-size: 14px; font-weight: bold; }\n";
    svg << "      .tick-label { font-size: 11px; }\n";
    svg << "      .legend-text { font-size: 12px; }\n";
    svg << "      .data-point { opacity: 0.8; }\n";
    svg << "    ]]>\n";
    svg << "  </style>\n";
    
    // Градиенты для линий
    svg << "  <linearGradient id=\"insertionGradient\" x1=\"0%\" y1=\"0%\" x2=\"100%\" y2=\"0%\">\n";
    svg << "    <stop offset=\"0%\" stop-color=\"" << colors.insertion << "\" stop-opacity=\"0.3\"/>\n";
    svg << "    <stop offset=\"100%\" stop-color=\"" << colors.insertion << "\" stop-opacity=\"0.1\"/>\n";
    svg << "  </linearGradient>\n";
    
    svg << "  <linearGradient id=\"heapGradient\" x1=\"0%\" y1=\"0%\" x2=\"100%\" y2=\"0%\">\n";
    svg << "    <stop offset=\"0%\" stop-color=\"" << colors.heap << "\" stop-opacity=\"0.3\"/>\n";
    svg << "    <stop offset=\"100%\" stop-color=\"" << colors.heap << "\" stop-opacity=\"0.1\"/>\n";
    svg << "  </linearGradient>\n";
    
    svg << "  <linearGradient id=\"stdGradient\" x1=\"0%\" y1=\"0%\" x2=\"100%\" y2=\"0%\">\n";
    svg << "    <stop offset=\"0%\" stop-color=\"" << colors.stdsort << "\" stop-opacity=\"0.3\"/>\n";
    svg << "    <stop offset=\"100%\" stop-color=\"" << colors.stdsort << "\" stop-opacity=\"0.1\"/>\n";
    svg << "  </linearGradient>\n";
    
    svg << "</defs>\n\n";
    
    // ==================== ФОН ====================
    svg << "<rect width=\"100%\" height=\"100%\" fill=\"" << colors.bg << "\"/>\n\n";
    
    // ==================== ЗАГОЛОВОК ====================
    svg << "<text x=\"" << (SVG_WIDTH / 2) << "\" y=\"40\" "
        << "text-anchor=\"middle\" class=\"title\" fill=\"" << colors.text << "\">\n";
    svg << "  Sorting Algorithms Performance Comparison\n";
    svg << "</text>\n\n";
    
    // ==================== ОСИ КООРДИНАТ ====================
    // Y ось (время)
    svg << "<line x1=\"" << MARGIN << "\" y1=\"" << MARGIN << "\" "
        << "x2=\"" << MARGIN << "\" y2=\"" << (SVG_HEIGHT - MARGIN) << "\" "
        << "stroke=\"" << colors.axis << "\" stroke-width=\"2\"/>\n";
    
    // X ось (размер)
    svg << "<line x1=\"" << MARGIN << "\" y1=\"" << (SVG_HEIGHT - MARGIN) << "\" "
        << "x2=\"" << (SVG_WIDTH - MARGIN) << "\" y2=\"" << (SVG_HEIGHT - MARGIN) << "\" "
        << "stroke=\"" << colors.axis << "\" stroke-width=\"2\"/>\n";
    
    // ==================== СЕТКА ====================
    // Вертикальные линии сетки
    int x_ticks = 5;
    for (int i = 0; i <= x_ticks; i++) {
        int x = MARGIN + (i * PLOT_WIDTH) / x_ticks;
        svg << "<line x1=\"" << x << "\" y1=\"" << MARGIN << "\" "
            << "x2=\"" << x << "\" y2=\"" << (SVG_HEIGHT - MARGIN) << "\" "
            << "stroke=\"" << colors.grid << "\" stroke-width=\"1\" stroke-dasharray=\"5,5\"/>\n";
    }
    
    // Горизонтальные линии сетки
    int y_ticks = 6;
    for (int i = 0; i <= y_ticks; i++) {
        int y = MARGIN + (i * PLOT_HEIGHT) / y_ticks;
        svg << "<line x1=\"" << MARGIN << "\" y1=\"" << y << "\" "
            << "x2=\"" << (SVG_WIDTH - MARGIN) << "\" y2=\"" << y << "\" "
            << "stroke=\"" << colors.grid << "\" stroke-width=\"1\" stroke-dasharray=\"5,5\"/>\n";
    }
    
    // ==================== ПОДПИСИ ОСЕЙ ====================
    // Подпись оси X
    svg << "<text x=\"" << (SVG_WIDTH / 2) << "\" y=\"" << (SVG_HEIGHT - 20) << "\" "
        << "text-anchor=\"middle\" class=\"axis-label\" fill=\"" << colors.text << "\">\n";
    svg << "  Array Size\n";
    svg << "</text>\n";
    
    // Подпись оси Y
    svg << "<text x=\"40\" y=\"" << (SVG_HEIGHT / 2) << "\" "
        << "text-anchor=\"middle\" class=\"axis-label\" fill=\"" << colors.text << "\" "
        << "transform=\"rotate(-90, 40, " << (SVG_HEIGHT / 2) << ")\">\n";
    svg << "  Time (microseconds)\n";
    svg << "</text>\n\n";
    
    // ==================== ШКАЛЫ ====================
    // Максимальное значение для масштабирования
    double max_time = 0;
    if (has_insertion_data) {
        max_time = std::max(max_time, get_max_value(insertion_times));
    }
    max_time = std::max(max_time, get_max_value(heap_times));
    max_time = std::max(max_time, get_max_value(std_times));
    
    if (max_time <= 0) max_time = 1.0;
    
    // Подписи по оси Y
    for (int i = 0; i <= y_ticks; i++) {
        double value = max_time * (y_ticks - i) / y_ticks;
        int y = MARGIN + (i * PLOT_HEIGHT) / y_ticks;
        
        svg << "<text x=\"" << (MARGIN - 10) << "\" y=\"" << (y + 4) << "\" "
            << "text-anchor=\"end\" class=\"tick-label\" fill=\"" << colors.text << "\">\n";
        svg << "  " << format_number(value) << "\n";
        svg << "</text>\n";
    }
    
    // Подписи по оси X
    double min_size = sizes.front();
    double max_size = sizes.back();
    
    for (int i = 0; i <= x_ticks; i++) {
        double size_value = min_size + (max_size - min_size) * i / x_ticks;
        int x = MARGIN + (i * PLOT_WIDTH) / x_ticks;
        
        svg << "<text x=\"" << x << "\" y=\"" << (SVG_HEIGHT - MARGIN + 20) << "\" "
            << "text-anchor=\"middle\" class=\"tick-label\" fill=\"" << colors.text << "\">\n";
        svg << "  " << static_cast<int>(size_value) << "\n";
        svg << "</text>\n";
    }
    
    // ==================== ГРАФИКИ ====================
    // Функция для преобразования координат
    auto map_x = [&](double size) -> double {
        return MARGIN + ((size - min_size) / (max_size - min_size)) * PLOT_WIDTH;
    };
    
    auto map_y = [&](double time) -> double {
        return MARGIN + (time / max_time) * PLOT_HEIGHT;
    };
    
    // Функция для рисования графика с заливкой
    auto draw_graph_with_area = [&](const std::vector<double>& times, 
                                    const char* line_color,
                                    const char* gradient_id,
                                    const std::string& id) {
        if (times.empty()) return;
        
        // Создаем путь для заливки
        svg << "<path id=\"area_" << id << "\" d=\"M ";
        
        // Начинаем с нижнего левого угла
        svg << map_x(min_size) << "," << map_y(0) << " ";
        
        // Вершины графика
        for (size_t i = 0; i < sizes.size(); i++) {
            if (times[i] > 0) {
                svg << "L " << map_x(sizes[i]) << "," << map_y(times[i]) << " ";
            }
        }
        
        // Заканчиваем в нижнем правом углу
        svg << "L " << map_x(max_size) << "," << map_y(0) << " ";
        svg << "Z\" fill=\"url(#" << gradient_id << ")\" stroke=\"none\"/>\n";
        
        // Линия графика
        svg << "<polyline fill=\"none\" stroke=\"" << line_color << "\" stroke-width=\"3\" "
            << "points=\"";
        
        for (size_t i = 0; i < sizes.size(); i++) {
            if (times[i] > 0) {
                svg << map_x(sizes[i]) << "," << map_y(times[i]) << " ";
            }
        }
        
        svg << "\"/>\n";
        
        // Точки данных
        for (size_t i = 0; i < sizes.size(); i++) {
            if (times[i] > 0) {
                svg << "<circle cx=\"" << map_x(sizes[i]) << "\" "
                    << "cy=\"" << map_y(times[i]) << "\" "
                    << "r=\"4\" fill=\"" << line_color << "\" class=\"data-point\"/>\n";
            }
        }
    };
    
    // Рисуем графики (в обратном порядке для правильного наложения)
    
    // 1. std::sort (фон)
    draw_graph_with_area(std_times, colors.stdsort, "stdGradient", "std");
    
    // 2. Heap Sort
    draw_graph_with_area(heap_times, colors.heap, "heapGradient", "heap");
    
    // 3. Insertion Sort (если есть)
    if (has_insertion_data) {
        draw_graph_with_area(insertion_times, colors.insertion, "insertionGradient", "insertion");
    }
    
    // ==================== ЛЕГЕНДА ====================
    int legend_x = SVG_WIDTH - 200;
    int legend_y = MARGIN + 20;
    int legend_item_height = 25;
    
    svg << "<rect x=\"" << (legend_x - 10) << "\" y=\"" << (legend_y - 10) << "\" "
        << "width=\"190\" height=\"";
    
    if (has_insertion_data) {
        svg << (legend_item_height * 3 + 10);
    } else {
        svg << (legend_item_height * 2 + 10);
    }
    
    svg << "\" fill=\"white\" stroke=\"" << colors.grid << "\" stroke-width=\"1\" "
        << "rx=\"5\" ry=\"5\"/>\n";
    
    svg << "<text x=\"" << (legend_x) << "\" y=\"" << legend_y << "\" "
        << "class=\"axis-label\" fill=\"" << colors.text << "\">Legend:</text>\n";
    
    int current_y = legend_y + legend_item_height;
    
    if (has_insertion_data) {
        // Insertion Sort
        svg << "<line x1=\"" << legend_x << "\" y1=\"" << (current_y - 5) << "\" "
            << "x2=\"" << (legend_x + 30) << "\" y2=\"" << (current_y - 5) << "\" "
            << "stroke=\"" << colors.insertion << "\" stroke-width=\"3\"/>\n";
        svg << "<circle cx=\"" << (legend_x + 15) << "\" cy=\"" << (current_y - 5) << "\" "
            << "r=\"4\" fill=\"" << colors.insertion << "\"/>\n";
        svg << "<text x=\"" << (legend_x + 40) << "\" y=\"" << current_y << "\" "
            << "class=\"legend-text\" fill=\"" << colors.text << "\">Insertion Sort</text>\n";
        
        current_y += legend_item_height;
    }
    
    // Heap Sort
    svg << "<line x1=\"" << legend_x << "\" y1=\"" << (current_y - 5) << "\" "
        << "x2=\"" << (legend_x + 30) << "\" y2=\"" << (current_y - 5) << "\" "
        << "stroke=\"" << colors.heap << "\" stroke-width=\"3\"/>\n";
    svg << "<circle cx=\"" << (legend_x + 15) << "\" cy=\"" << (current_y - 5) << "\" "
        << "r=\"4\" fill=\"" << colors.heap << "\"/>\n";
    svg << "<text x=\"" << (legend_x + 40) << "\" y=\"" << current_y << "\" "
        << "class=\"legend-text\" fill=\"" << colors.text << "\">Heap Sort</text>\n";
    
    current_y += legend_item_height;
    
    // std::sort
    svg << "<line x1=\"" << legend_x << "\" y1=\"" << (current_y - 5) << "\" "
        << "x2=\"" << (legend_x + 30) << "\" y2=\"" << (current_y - 5) << "\" "
        << "stroke=\"" << colors.stdsort << "\" stroke-width=\"3\"/>\n";
    svg << "<circle cx=\"" << (legend_x + 15) << "\" cy=\"" << (current_y - 5) << "\" "
        << "r=\"4\" fill=\"" << colors.stdsort << "\"/>\n";
    svg << "<text x=\"" << (legend_x + 40) << "\" y=\"" << current_y << "\" "
        << "class=\"legend-text\" fill=\"" << colors.text << "\">std::sort</text>\n";
    
    // ==================== ИНФОРМАЦИЯ О ДАННЫХ ====================
    svg << "<text x=\"" << MARGIN << "\" y=\"30\" "
        << "class=\"tick-label\" fill=\"" << colors.text << "\">\n";
    svg << "  Data points: " << sizes.size() << ", Max time: " << format_number(max_time) << "\n";
    svg << "</text>\n";
    
    // ==================== КОНЕЦ SVG ====================
    svg << "</svg>\n";
    
    svg.close();
    
    std::cout << "\n✓ SVG plot created successfully: " << output_svg << "\n";
    std::cout << "  Size: " << SVG_WIDTH << "x" << SVG_HEIGHT << " pixels\n";
    std::cout << "  Open in any web browser to view\n";
    
    return true;
}

bool SvgPlotter::create_speedup_svg(const std::string& csv_filename,
                                   const std::string& output_svg) {
    
    std::ifstream csv(csv_filename);
    if (!csv.is_open()) {
        std::cerr << "Error: Cannot open CSV file: " << csv_filename << "\n";
        return false;
    }
    
    // Читаем данные
    std::vector<double> sizes;
    std::vector<double> insertion_times;
    std::vector<double> heap_times;
    std::vector<double> std_times;
    
    std::string line;
    std::getline(csv, line);
    
    while (std::getline(csv, line)) {
        std::stringstream ss(line);
        std::string value;
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
                // Пропускаем некорректные строки
            }
        }
    }
    
    csv.close();
    
    if (sizes.empty()) {
        std::cerr << "Error: No valid data found in CSV file\n";
        return false;
    }
    
    // Вычисляем ускорение
    std::vector<double> insertion_vs_heap;
    std::vector<double> heap_vs_std;
    
    bool has_insertion_data = false;
    
    for (size_t i = 0; i < sizes.size(); i++) {
        if (insertion_times[i] > 0 && heap_times[i] > 0) {
            insertion_vs_heap.push_back(insertion_times[i] / heap_times[i]);
            has_insertion_data = true;
        } else {
            insertion_vs_heap.push_back(-1);
        }
        
        if (heap_times[i] > 0 && std_times[i] > 0) {
            heap_vs_std.push_back(heap_times[i] / std_times[i]);
        } else {
            heap_vs_std.push_back(-1);
        }
    }
    
    // Создаем SVG файл
    std::ofstream svg(output_svg);
    if (!svg.is_open()) {
        std::cerr << "Error: Cannot create SVG file: " << output_svg << "\n";
        return false;
    }
    
    const int SPEEDUP_SVG_WIDTH = 900;
    const int SPEEDUP_SVG_HEIGHT = 500;
    const int SPEEDUP_MARGIN = 60;
    
    // ==================== НАЧАЛО SVG ====================
    svg << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    svg << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">\n";
    svg << "<svg width=\"" << SPEEDUP_SVG_WIDTH << "\" height=\"" << SPEEDUP_SVG_HEIGHT << "\" "
        << "xmlns=\"http://www.w3.org/2000/svg\">\n\n";
    
    // Стили
    svg << "<defs>\n";
    svg << "  <style type=\"text/css\">\n";
    svg << "    <![CDATA[\n";
    svg << "      text { font-family: Arial, sans-serif; }\n";
    svg << "      .title { font-size: 16px; font-weight: bold; }\n";
    svg << "      .axis-label { font-size: 12px; }\n";
    svg << "      .baseline { stroke-dasharray: 5,5; }\n";
    svg << "    ]]>\n";
    svg << "  </style>\n";
    svg << "</defs>\n\n";
    
    // Фон
    svg << "<rect width=\"100%\" height=\"100%\" fill=\"white\"/>\n\n";
    
    // Заголовок
    svg << "<text x=\"" << (SPEEDUP_SVG_WIDTH / 2) << "\" y=\"30\" "
        << "text-anchor=\"middle\" class=\"title\" fill=\"#333333\">\n";
    svg << "  Algorithm Speedup Comparison\n";
    svg << "</text>\n\n";
    
    // Подзаголовок
    svg << "<text x=\"" << (SPEEDUP_SVG_WIDTH / 2) << "\" y=\"50\" "
        << "text-anchor=\"middle\" font-size=\"11\" fill=\"#666666\">\n";
    svg << "  (Values > 1: first algorithm is slower)\n";
    svg << "</text>\n\n";
    
    // Область графика
    int speedup_plot_width = SPEEDUP_SVG_WIDTH - 2 * SPEEDUP_MARGIN;
    int speedup_plot_height = SPEEDUP_SVG_HEIGHT - 2 * SPEEDUP_MARGIN;
    
    // Оси
    svg << "<line x1=\"" << SPEEDUP_MARGIN << "\" y1=\"" << (SPEEDUP_SVG_HEIGHT - SPEEDUP_MARGIN) << "\" "
        << "x2=\"" << (SPEEDUP_SVG_WIDTH - SPEEDUP_MARGIN) << "\" y2=\"" << (SPEEDUP_SVG_HEIGHT - SPEEDUP_MARGIN) << "\" "
        << "stroke=\"black\" stroke-width=\"2\"/>\n";
    
    svg << "<line x1=\"" << SPEEDUP_MARGIN << "\" y1=\"" << SPEEDUP_MARGIN << "\" "
        << "x2=\"" << SPEEDUP_MARGIN << "\" y2=\"" << (SPEEDUP_SVG_HEIGHT - SPEEDUP_MARGIN) << "\" "
        << "stroke=\"black\" stroke-width=\"2\"/>\n";
    
    // Базовая линия (1.0)
    double baseline_y = SPEEDUP_MARGIN + (speedup_plot_height / 2.0);
    svg << "<line x1=\"" << SPEEDUP_MARGIN << "\" y1=\"" << baseline_y << "\" "
        << "x2=\"" << (SPEEDUP_SVG_WIDTH - SPEEDUP_MARGIN) << "\" y2=\"" << baseline_y << "\" "
        << "stroke=\"#666666\" stroke-width=\"1\" class=\"baseline\"/>\n";
    
    svg << "<text x=\"" << (SPEEDUP_MARGIN - 10) << "\" y=\"" << (baseline_y + 4) << "\" "
        << "text-anchor=\"end\" font-size=\"10\" fill=\"#666666\">1.0</text>\n";
    
    // Масштабирование
    double min_size = sizes.front();
    double max_size = sizes.back();
    
    auto map_speedup_x = [&](double size) -> double {
        return SPEEDUP_MARGIN + ((size - min_size) / (max_size - min_size)) * speedup_plot_width;
    };
    
    auto map_speedup_y = [&](double ratio) -> double {
        // Отображаем 0-2 в область графика
        double normalized = std::max(0.0, std::min(2.0, ratio));
        return SPEEDUP_MARGIN + (normalized / 2.0) * speedup_plot_height;
    };
    
    // Рисуем ускорение
    for (size_t i = 0; i < sizes.size(); i++) {
        double x = map_speedup_x(sizes[i]);
        
        // Heap/std::sort
        if (heap_vs_std[i] > 0) {
            double y = map_speedup_y(heap_vs_std[i]);
            
            svg << "<circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"5\" "
                << "fill=\"#FF8800\" stroke=\"#CC6600\" stroke-width=\"1\"/>\n";
            
            // Подпись значения
            if (i == 0 || i == sizes.size() - 1 || i == sizes.size() / 2) {
                svg << "<text x=\"" << x << "\" y=\"" << (y - 10) << "\" "
                    << "text-anchor=\"middle\" font-size=\"9\" fill=\"#CC6600\">\n";
                svg << "  " << std::fixed << std::setprecision(2) << heap_vs_std[i] << "x\n";
                svg << "</text>\n";
            }
        }
        
        // Insertion/Heap (если есть)
        if (has_insertion_data && insertion_vs_heap[i] > 0) {
            double y = map_speedup_y(insertion_vs_heap[i]);
            
            svg << "<circle cx=\"" << x << "\" cy=\"" << y << "\" r=\"5\" "
                << "fill=\"#AA44AA\" stroke=\"#882288\" stroke-width=\"1\"/>\n";
        }
    }
    
    // Соединяем точки линиями
    svg << "<polyline fill=\"none\" stroke=\"#FF8800\" stroke-width=\"2\" "
        << "stroke-opacity=\"0.7\" points=\"";
    
    for (size_t i = 0; i < sizes.size(); i++) {
        if (heap_vs_std[i] > 0) {
            svg << map_speedup_x(sizes[i]) << "," << map_speedup_y(heap_vs_std[i]) << " ";
        }
    }
    
    svg << "\"/>\n";
    
    if (has_insertion_data) {
        svg << "<polyline fill=\"none\" stroke=\"#AA44AA\" stroke-width=\"2\" "
            << "stroke-opacity=\"0.7\" points=\"";
        
        for (size_t i = 0; i < sizes.size(); i++) {
            if (insertion_vs_heap[i] > 0) {
                svg << map_speedup_x(sizes[i]) << "," << map_speedup_y(insertion_vs_heap[i]) << " ";
            }
        }
        
        svg << "\"/>\n";
    }
    
    // Легенда
    int legend_y = 80;
    
    if (has_insertion_data) {
        svg << "<circle cx=\"100\" cy=\"" << legend_y << "\" r=\"5\" fill=\"#AA44AA\" stroke=\"#882288\"/>\n";
        svg << "<text x=\"115\" y=\"" << (legend_y + 4) << "\" font-size=\"11\">Insertion Sort / Heap Sort</text>\n";
        legend_y += 25;
    }
    
    svg << "<circle cx=\"100\" cy=\"" << legend_y << "\" r=\"5\" fill=\"#FF8800\" stroke=\"#CC6600\"/>\n";
    svg << "<text x=\"115\" y=\"" << (legend_y + 4) << "\" font-size=\"11\">Heap Sort / std::sort</text>\n";
    
    svg << "<line x1=\"100\" y1=\"" << (legend_y + 25) << "\" x2=\"130\" y2=\"" << (legend_y + 25) << "\" "
        << "stroke=\"#666666\" stroke-width=\"1\" class=\"baseline\"/>\n";
    svg << "<text x=\"140\" y=\"" << (legend_y + 29) << "\" font-size=\"11\">Baseline (1.0)</text>\n";
    
    // Подписи осей
    svg << "<text x=\"" << (SPEEDUP_SVG_WIDTH / 2) << "\" y=\"" << (SPEEDUP_SVG_HEIGHT - 20) << "\" "
        << "text-anchor=\"middle\" class=\"axis-label\">Array Size</text>\n";
    
    svg << "<text x=\"30\" y=\"" << (SPEEDUP_SVG_HEIGHT / 2) << "\" "
        << "text-anchor=\"middle\" class=\"axis-label\" "
        << "transform=\"rotate(-90, 30, " << (SPEEDUP_SVG_HEIGHT / 2) << ")\">\n";
    svg << "  Speedup Ratio\n";
    svg << "</text>\n";
    
    svg << "</svg>\n";
    
    svg.close();
    
    std::cout << "✓ Speedup SVG plot created: " << output_svg << "\n";
    
    return true;
}

bool SvgPlotter::create_all_svg_plots(const std::string& csv_filename) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "GENERATING SVG PLOTS (NO DEPENDENCIES NEEDED!)\n";
    std::cout << std::string(60, '=') << "\n";
    
    std::ifstream test_file(csv_filename);
    if (!test_file.is_open()) {
        std::cerr << "✗ ERROR: Cannot open CSV file: " << csv_filename << "\n";
        return false;
    }
    test_file.close();
    
    std::cout << "✓ Processing data from: " << csv_filename << "\n\n";
    
    bool success1 = create_performance_svg(csv_filename);
    bool success2 = create_speedup_svg(csv_filename);
    
    if (success1 || success2) {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "SVG PLOTS CREATED SUCCESSFULLY!\n";
        std::cout << std::string(60, '=') << "\n\n";
        
        if (success1) {
            std::cout << "📊 Main Performance Plot:\n";
            std::cout << "   File: performance_plot.svg\n";
            std::cout << "   Open in: Any web browser (Chrome, Firefox, Edge, etc.)\n";
            std::cout << "   Features: Interactive zoom, save as PNG/PDF\n\n";
        }
        
        if (success2) {
            std::cout << "📈 Speedup Analysis Plot:\n";
            std::cout << "   File: speedup_plot.svg\n";
            std::cout << "   Open in: Any web browser\n\n";
        }
        
        std::cout << "✨ ADVANTAGES OF SVG:\n";
        std::cout << "   1. No software installation needed\n";
        std::cout << "   2. High quality vector graphics\n";
        std::cout << "   3. Scalable to any size without quality loss\n";
        std::cout << "   4. Can be embedded in HTML reports\n";
        std::cout << "   5. Can be converted to PDF/PNG if needed\n";
        std::cout << "\n" << std::string(60, '=') << "\n";
        
        return true;
    }
    
    return false;
}

} // namespace coursework