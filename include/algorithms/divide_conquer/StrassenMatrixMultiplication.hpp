#pragma once

#include "algorithms/divide_conquer/ClassicalMatrixMultiplication.hpp"

namespace project31::algorithms {

[[nodiscard]] Matrix strassenMatrixMultiply(const Matrix& left, const Matrix& right);

} // namespace project31::algorithms
