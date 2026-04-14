#include <iostream>
#include <vector>

/*
 * Plagiarized version of bubble sort
 * Variables renamed and block comments used instead
 */
void sort_array(std::vector<int>& numbers) {
    int length = numbers.size();
    for (int outer = 0; outer < length - 1; outer++) {
        for (int inner = 0; inner < length - outer - 1; inner++) {
            if (numbers[inner] > numbers[inner + 1]) {
                /* Swap logic */
                int tmp = numbers[inner];
                numbers[inner] = numbers[inner + 1];
                numbers[inner + 1] = tmp;
            }
        }
    }
}

int main() {
    std::vector<int> array_to_sort = {64, 34, 25, 12, 22, 11, 90};
    sort_array(array_to_sort);
    for(int el : array_to_sort) std::cout << el << " ";
    return 0;
}
