#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace project31::algorithms {

[[nodiscard]] std::optional<std::vector<std::size_t>> findSubsetSum(
    const std::vector<std::int64_t>& values,
    std::int64_t target
);

} // namespace project31::algorithms
