#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace project31::algorithms {

struct MatrixChainResult {
    std::int64_t minimumCost;
    std::string parenthesization;
};

[[nodiscard]] MatrixChainResult matrixChainMultiplication(
    const std::vector<std::int64_t>& dimensions
);

} // namespace project31::algorithms
