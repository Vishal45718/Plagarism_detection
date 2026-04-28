#include <iostream>
#include <vector>
#include <algorithm>

int calculate_sum(const std::vector<int>& numbers) {
    int total = 0;
    for (int num : numbers) {
        total += num;
    }
    return total;
}

double calculate_average(const std::vector<int>& numbers) {
    if (numbers.empty()) return 0.0;
    int sum = calculate_sum(numbers);
    return static_cast<double>(sum) / numbers.size();
}

int main() {
    std::vector<int> data = {10, 20, 30, 40, 50};
    std::cout << "Sum: " << calculate_sum(data) << "\n";
    std::cout << "Average: " << calculate_average(data) << "\n";
    return 0;
}
