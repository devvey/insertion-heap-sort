#pragma once
#include <vector>
#include <cstddef>
#include "benchmark.hpp"  // теперь DataType берется отсюда

namespace coursework {

class ArrayGenerator {
public:
    std::vector<int> generate(size_t size, DataType type);
};

} // namespace coursework