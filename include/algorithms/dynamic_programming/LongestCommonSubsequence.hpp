#pragma once

#include <cstddef>
#include <string>

namespace project31::algorithms {

struct LongestCommonSubsequenceResult {
    std::size_t length;
    std::string subsequence;
};

[[nodiscard]] LongestCommonSubsequenceResult longestCommonSubsequence(
    const std::string& first,
    const std::string& second
);

} // namespace project31::algorithms
