#pragma once

#include <cstddef>
#include <optional>
#include <vector>

namespace project31::algorithms {

[[nodiscard]] std::optional<std::size_t> findCelebrity(
    const std::vector<std::vector<int>>& knows
);

} // namespace project31::algorithms
