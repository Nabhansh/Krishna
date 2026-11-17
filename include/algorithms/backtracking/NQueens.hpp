#pragma once

#include <cstddef>
#include <vector>

namespace project31::algorithms {

inline constexpr std::size_t kMaximumEnumeratedQueens = 12;

[[nodiscard]] std::vector<std::vector<std::size_t>> solveNQueens(
    std::size_t boardSize
);

} // namespace project31::algorithms
