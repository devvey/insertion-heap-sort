#include "algorithms.hpp"

// Явные инстанциации для часто используемых типов
template void coursework::insertion_sort<std::vector<int>::iterator>(
    std::vector<int>::iterator, std::vector<int>::iterator);
template void coursework::heap_sort<std::vector<int>::iterator>(
    std::vector<int>::iterator, std::vector<int>::iterator);