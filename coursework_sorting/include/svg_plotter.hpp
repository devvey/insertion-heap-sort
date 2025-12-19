// svg_plotter.hpp
#pragma once

#include <string>
#include <vector>

namespace coursework {

class SvgPlotter {
private:
    // Константы для размеров SVG
    static const int SVG_WIDTH;
    static const int SVG_HEIGHT;
    static const int MARGIN;
    static const int PLOT_WIDTH;
    static const int PLOT_HEIGHT;
    
    // Структура для цветов
    struct Color {
        const char* insertion;
        const char* heap;
        const char* stdsort;
        const char* grid;
        const char* axis;
        const char* text;
        const char* bg;
    };
    
    // Вспомогательные функции
    static double get_max_value(const std::vector<double>& data);
    static std::string format_number(double num);
    
public:
    // Основная функция создания SVG графика
    static bool create_performance_svg(const std::string& csv_filename,
                                      const std::string& output_svg = "performance_plot.svg");
    
    // Создание SVG для графика ускорения
    static bool create_speedup_svg(const std::string& csv_filename,
                                  const std::string& output_svg = "speedup_plot.svg");
    
    // Создание всех SVG графиков
    static bool create_all_svg_plots(const std::string& csv_filename);
};

} // namespace coursework