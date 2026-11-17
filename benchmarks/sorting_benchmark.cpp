#include "algorithms/sorting/MergeSort.hpp"
#include "algorithms/sorting/QuickSort.hpp"

#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {

std::vector<std::string> passengerIds(std::size_t count) {
    std::vector<std::string> ids;
    ids.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        const std::size_t permuted = (index * 7919U) % count;
        std::string id = "P-";
        const std::string number = std::to_string(permuted);
        id.append(6U - (number.size() < 6U ? number.size() : 6U), '0');
        id += number;
        ids.push_back(std::move(id));
    }
    return ids;
}

template <typename SortFunction>
double measureMilliseconds(
    const std::vector<std::string>& input,
    SortFunction sortFunction
) {
    std::vector<std::string> values = input;
    const auto start = std::chrono::steady_clock::now();
    sortFunction(values);
    const auto finish = std::chrono::steady_clock::now();

    for (std::size_t index = 1; index < values.size(); ++index) {
        if (values[index] < values[index - 1]) {
            std::cerr << "Sort validation failed.\n";
            return -1.0;
        }
    }
    return std::chrono::duration<double, std::milli>(finish - start).count();
}

} // namespace

int main() {
    const std::vector<std::pair<std::string, std::size_t>> datasets{
        {"small", 100},
        {"medium", 5'000},
        {"large", 50'000}
    };

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Passenger ID sorting benchmark (milliseconds)\n"
                 "Dataset    Records      Merge Sort     Quick Sort\n";
    for (const auto& [label, size] : datasets) {
        const std::vector<std::string> input = passengerIds(size);
        const double mergeMilliseconds = measureMilliseconds(
            input,
            [](std::vector<std::string>& values) {
                project31::algorithms::mergeSort(values);
            }
        );
        const double quickMilliseconds = measureMilliseconds(
            input,
            [](std::vector<std::string>& values) {
                project31::algorithms::quickSort(values);
            }
        );
        std::cout << std::left << std::setw(11) << label
                  << std::right << std::setw(8) << size
                  << std::setw(16) << mergeMilliseconds
                  << std::setw(16) << quickMilliseconds << '\n';
        if (mergeMilliseconds < 0.0 || quickMilliseconds < 0.0) {
            return 1;
        }
    }
    return 0;
}
