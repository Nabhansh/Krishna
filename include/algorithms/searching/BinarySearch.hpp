#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace project31::algorithms {

[[nodiscard]] std::optional<std::size_t> iterativeBinarySearch(
    const std::vector<int>& values,
    int target
);
[[nodiscard]] std::optional<std::size_t> recursiveBinarySearch(
    const std::vector<int>& values,
    int target
);
[[nodiscard]] std::optional<std::size_t> iterativeBinarySearch(
    const std::vector<std::string>& values,
    const std::string& target
);
[[nodiscard]] std::optional<std::size_t> recursiveBinarySearch(
    const std::vector<std::string>& values,
    const std::string& target
);
[[nodiscard]] std::size_t lowerBound(
    const std::vector<std::string>& values,
    const std::string& target
);

} // namespace project31::algorithms
