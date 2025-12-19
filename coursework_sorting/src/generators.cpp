#include "generators.hpp"
#include <random>
#include <algorithm>
#include <chrono>  // для получения сида

namespace coursework {

std::vector<int> ArrayGenerator::generate(size_t size, DataType type) {
    std::vector<int> data(size);
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(-1000, 1000);  // увеличил диапазон

    switch(type) {
        case DataType::RANDOM:
            for(auto &x : data) x = dist(rng);
            break;
        case DataType::SORTED:
            for(size_t i = 0; i < size; ++i) data[i] = static_cast<int>(i);
            break;
        case DataType::REVERSED:
            for(size_t i = 0; i < size; ++i) data[i] = static_cast<int>(size - i - 1);
            break;
        case DataType::ALMOST_SORTED:
            for(size_t i = 0; i < size; ++i) data[i] = static_cast<int>(i);
            if(size > 10) {
                // Делаем несколько инверсий
                for(int j = 0; j < 5 && j < static_cast<int>(size)/2; ++j) {
                    std::swap(data[j], data[size - j - 1]);
                }
            }
            break;
    }
    return data;
}

} // namespace coursework