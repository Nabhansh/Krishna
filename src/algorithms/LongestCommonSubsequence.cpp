#include "algorithms/dynamic_programming/LongestCommonSubsequence.hpp"

#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace project31::algorithms {

LongestCommonSubsequenceResult longestCommonSubsequence(
    const std::string& first,
    const std::string& second
) {
    if (first.size() == std::numeric_limits<std::size_t>::max() ||
        second.size() == std::numeric_limits<std::size_t>::max()) {
        throw std::length_error("LCS table dimensions exceed the supported range.");
    }

    const std::size_t rows = first.size() + 1;
    const std::size_t columns = second.size() + 1;
    if (rows > std::numeric_limits<std::size_t>::max() / columns ||
        rows > std::vector<std::vector<std::size_t>>().max_size() ||
        columns > std::vector<std::size_t>().max_size()) {
        throw std::length_error("LCS table dimensions exceed the supported range.");
    }

    // dp[i][j] is the LCS length for the first i and j characters.
    std::vector<std::vector<std::size_t>> dp(
        rows,
        std::vector<std::size_t>(columns, 0)
    );
    for (std::size_t i = 1; i < rows; ++i) {
        for (std::size_t j = 1; j < columns; ++j) {
            if (first[i - 1] == second[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else if (dp[i - 1][j] > dp[i][j - 1]) {
                dp[i][j] = dp[i - 1][j];
            } else {
                dp[i][j] = dp[i][j - 1];
            }
        }
    }

    std::string subsequence;
    subsequence.reserve(dp.back().back());
    std::size_t i = first.size();
    std::size_t j = second.size();
    while (i > 0 && j > 0) {
        if (first[i - 1] == second[j - 1]) {
            subsequence.push_back(first[i - 1]);
            --i;
            --j;
        } else if (dp[i - 1][j] > dp[i][j - 1]) {
            --i;
        } else {
            --j;
        }
    }
    for (std::size_t left = 0, right = subsequence.size(); left < right / 2; ++left) {
        std::swap(subsequence[left], subsequence[right - left - 1]);
    }
    return {dp.back().back(), std::move(subsequence)};
}

} // namespace project31::algorithms
