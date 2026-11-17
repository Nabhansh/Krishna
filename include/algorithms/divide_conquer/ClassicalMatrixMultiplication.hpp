#pragma once

#include <vector>

namespace project31::algorithms {

using Matrix = std::vector<std::vector<double>>;

[[nodiscard]] Matrix classicalMatrixMultiply(const Matrix& left, const Matrix& right);

} // namespace project31::algorithms
