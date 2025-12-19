cmake_minimum_required(VERSION 3.15)
project(coursework_sorting VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Уровень оптимизации для Release сборки
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# Основная цель - исполняемый файл для курсовой
add_executable(coursework_sorting
    src/algorithms.cpp
    src/generators.cpp
    src/benchmark.cpp
    src/svg_plotter.cpp
    main.cpp
)

# Подключаем папку include
target_include_directories(coursework_sorting PRIVATE include)

# Для Windows: отключаем WX (предупреждения как ошибки)
if(MSVC)
    target_compile_options(coursework_sorting PRIVATE /W4 /WX-)
else()
    target_compile_options(coursework_sorting PRIVATE -Wall -Wextra -Wpedantic)
endif()

message(STATUS "Project configured successfully!")