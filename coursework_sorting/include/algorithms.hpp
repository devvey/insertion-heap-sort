#pragma once
#include <vector>
#include <iterator>
#include <algorithm>

namespace coursework {

// Insertion Sort
template<typename Iter>
void insertion_sort(Iter begin, Iter end) {
    if (begin == end) return;
    for (Iter i = begin + 1; i != end; ++i) {
        auto key = *i;
        Iter j = i;
        while (j != begin && *(j - 1) > key) {
            *j = *(j - 1);
            --j;
        }
        *j = key;
    }
}

// Вспомогательная функция для Heap Sort
template<typename Iter>
void heapify(Iter begin, Iter end, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && *(begin + left) > *(begin + largest))
        largest = left;
    if (right < n && *(begin + right) > *(begin + largest))
        largest = right;

    if (largest != i) {
        std::swap(*(begin + i), *(begin + largest));
        heapify(begin, end, n, largest);
    }
}

// Heap Sort
template<typename Iter>
void heap_sort(Iter begin, Iter end) {
    auto n = std::distance(begin, end);
    if (n <= 1) return;

    // Построение кучи
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(begin, end, n, i);

    // Извлечение элементов из кучи
    for (int i = n - 1; i > 0; i--) {
        std::swap(*begin, *(begin + i));
        heapify(begin, end, i, 0);
    }
}

} // namespace coursework