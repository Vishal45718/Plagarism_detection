#include <iostream>
#include <vector>
#include <algorithm>

int compute_total(const std::vector<int>& values) {
    int res = 0;
    for (int v : values) {
        res += v;
    }
    return res;
}

double compute_mean(const std::vector<int>& values) {
    if (values.empty()) return 0.0;
    int s = compute_total(values);
    return static_cast<double>(s) / values.size();
}

int main() {
    std::vector<int> my_list = {10, 20, 30, 40, 50};
    std::cout << "Sum: " << compute_total(my_list) << "\n";
    std::cout << "Average: " << compute_mean(my_list) << "\n";
    return 0;
}
