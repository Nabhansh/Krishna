#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

namespace project31::algorithms {

inline constexpr std::size_t kMaximumHeldKarpVertices = 18;

struct BoundedTravellingSalespersonResult {
    std::int64_t totalCost;
    std::vector<std::size_t> tour;
};

[[nodiscard]] std::optional<BoundedTravellingSalespersonResult>
solveBoundedTravellingSalesperson(
    const std::vector<std::vector<std::optional<std::int64_t>>>& costs,
    std::size_t startVertex = 0
);

} // namespace project31::algorithms
