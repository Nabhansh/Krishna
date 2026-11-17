#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace project31::algorithms {

struct ChineseRemainderResult {
    std::int64_t solution;
    std::int64_t combinedModulus;
};

// Empty systems return the identity solution 0 modulo 1.
[[nodiscard]] ChineseRemainderResult solveChineseRemainderTheorem(
    const std::vector<std::int64_t>& residues,
    const std::vector<std::int64_t>& moduli
);

} // namespace project31::algorithms
