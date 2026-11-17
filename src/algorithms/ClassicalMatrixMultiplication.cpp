#include "algorithms/divide_conquer/ClassicalMatrixMultiplication.hpp"

#include <cmath>
#include <stdexcept>

namespace project31::algorithms {
namespace {

std::size_t validateMatrix(const Matrix& matrix) {
    if (matrix.empty()) {
        return 0;
    }
    const std::size_t columns = matrix.front().size();
    for (const auto& row : matrix) {
        if (row.size() != columns) {
            throw std::invalid_argument("Matrix rows must have equal length.");
        }
        for (double value : row) {
            if (!std::isfinite(value)) {
                throw std::invalid_argument("Matrix values must be finite.");
            }
        }
    }
    return columns;
}

} // namespace

Matrix classicalMatrixMultiply(const Matrix& left, const Matrix& right) {
    const std::size_t inner = validateMatrix(left);
    const std::size_t rightColumns = validateMatrix(right);
    if (inner != right.size()) {
        throw std::invalid_argument("Matrix dimensions are incompatible for multiplication.");
    }

    Matrix product(left.size(), std::vector<double>(rightColumns, 0.0));
    for (std::size_t row = 0; row < left.size(); ++row) {
        for (std::size_t k = 0; k < inner; ++k) {
            for (std::size_t column = 0; column < rightColumns; ++column) {
                product[row][column] += left[row][k] * right[k][column];
                if (!std::isfinite(product[row][column])) {
                    throw std::overflow_error("Matrix product exceeds floating-point range.");
                }
            }
        }
    }
    return product;
}

} // namespace project31::algorithms
