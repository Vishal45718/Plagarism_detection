#include <iostream>
#include <vector>

// Different code here
void say_hello() {
    std::cout << "Hello World!\n";
}

// Copied code
int calculate_sum(const std::vector<int>& numbers) {
    int total = 0;
    for (int num : numbers) {
        total += num;
    }
    return total;
}

int main() {
    say_hello();
    std::vector<int> d = {1, 2, 3};
    std::cout << calculate_sum(d) << "\n";
    return 0;
}
