#include <iostream>
#include <vector>
#include <algorithm>

double calculate_average(const std::vector<int>& numbers) {
    if (numbers.empty()) return 0.0;
    int total = 0;
    for (int num : numbers) {
        total += num;
    }
    return static_cast<double>(total) / numbers.size();
}

int calculate_sum(const std::vector<int>& numbers) {
    int total = 0;
    for (int num : numbers) {
        total += num;
    }
    return total;
}

int main() {
    std::vector<int> data = {10, 20, 30, 40, 50};
    std::cout << "Average: " << calculate_average(data) << "\n";
    std::cout << "Sum: " << calculate_sum(data) << "\n";
    return 0;
}
