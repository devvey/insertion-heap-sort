#include "algorithms.hpp"
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    std::cout << "Quick correctness test...\n";
    
    // Тест 1
    std::vector<int> test1 = {5, 2, 4, 6, 1, 3};
    std::vector<int> copy1 = test1;
    std::vector<int> copy2 = test1;
    
    coursework::insertion_sort(copy1.begin(), copy1.end());
    coursework::heap_sort(copy2.begin(), copy2.end());
    
    bool ok1 = std::is_sorted(copy1.begin(), copy1.end());
    bool ok2 = std::is_sorted(copy2.begin(), copy2.end());
    
    if (ok1 && ok2) {
        std::cout << "✓ Both algorithms work correctly\n";
        return 0;
    } else {
        std::cout << "✗ Error in algorithms\n";
        return 1;
    }
}