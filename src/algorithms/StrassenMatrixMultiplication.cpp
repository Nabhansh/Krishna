#include "algorithms/divide_conquer/StrassenMatrixMultiplication.hpp"

#include <cmath>
#include <limits>
#include <stdexcept>
#include <vector>

namespace project31::algorithms {
namespace {

using FlatMatrix = std::vector<double>;

std::size_t validate(const Matrix& matrix) {
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

FlatMatrix add(
    const FlatMatrix& left,
    const FlatMatrix& right,
    std::size_t size,
    bool subtract
) {
    FlatMatrix result(size * size);
    for (std::size_t index = 0; index < result.size(); ++index) {
        result[index] = subtract ? left[index] - right[index] : left[index] + right[index];
        if (!std::isfinite(result[index])) {
            throw std::overflow_error("Strassen intermediate exceeds floating-point range.");
        }
    }
    return result;
}

FlatMatrix strassen(const FlatMatrix& left, const FlatMatrix& right, std::size_t size) {
    if (size <= 32) {
        FlatMatrix product(size * size, 0.0);
        for (std::size_t row = 0; row < size; ++row) {
            for (std::size_t k = 0; k < size; ++k) {
                for (std::size_t column = 0; column < size; ++column) {
                    product[row * size + column] +=
                        left[row * size + k] * right[k * size + column];
                }
            }
        }
        for (double value : product) {
            if (!std::isfinite(value)) {
                throw std::overflow_error("Strassen product exceeds floating-point range.");
            }
        }
        return product;
    }

    const std::size_t half = size / 2;
    const auto quadrant = [size, half](const FlatMatrix& matrix, std::size_t rowOffset,
                                       std::size_t columnOffset) {
        FlatMatrix result(half * half);
        for (std::size_t row = 0; row < half; ++row) {
            for (std::size_t column = 0; column < half; ++column) {
                result[row * half + column] =
                    matrix[(row + rowOffset) * size + column + columnOffset];
            }
        }
        return result;
    };
    const FlatMatrix a = quadrant(left, 0, 0);
    const FlatMatrix b = quadrant(left, 0, half);
    const FlatMatrix c = quadrant(left, half, 0);
    const FlatMatrix d = quadrant(left, half, half);
    const FlatMatrix e = quadrant(right, 0, 0);
    const FlatMatrix f = quadrant(right, 0, half);
    const FlatMatrix g = quadrant(right, half, 0);
    const FlatMatrix h = quadrant(right, half, half);

    const FlatMatrix p1 = strassen(a, add(f, h, half, true), half);
    const FlatMatrix p2 = strassen(add(a, b, half, false), h, half);
    const FlatMatrix p3 = strassen(add(c, d, half, false), e, half);
    const FlatMatrix p4 = strassen(d, add(g, e, half, true), half);
    const FlatMatrix p5 = strassen(add(a, d, half, false), add(e, h, half, false), half);
    const FlatMatrix p6 = strassen(add(b, d, half, true), add(g, h, half, false), half);
    const FlatMatrix p7 = strassen(add(a, c, half, true), add(e, f, half, false), half);

    const FlatMatrix c11 = add(add(add(p5, p4, half, false), p2, half, true), p6, half, false);
    const FlatMatrix c12 = add(p1, p2, half, false);
    const FlatMatrix c21 = add(p3, p4, half, false);
    const FlatMatrix c22 = add(add(add(p1, p5, half, false), p3, half, true), p7, half, true);
    FlatMatrix result(size * size);
    for (std::size_t row = 0; row < half; ++row) {
        for (std::size_t column = 0; column < half; ++column) {
            result[row * size + column] = c11[row * half + column];
            result[row * size + column + half] = c12[row * half + column];
            result[(row + half) * size + column] = c21[row * half + column];
            result[(row + half) * size + column + half] = c22[row * half + column];
        }
    }
    return result;
}

} // namespace

Matrix strassenMatrixMultiply(const Matrix& left, const Matrix& right) {
    const std::size_t inner = validate(left);
    const std::size_t rightColumns = validate(right);
    if (inner != right.size()) {
        throw std::invalid_argument("Matrix dimensions are incompatible for multiplication.");
    }
    if (left.empty() || rightColumns == 0 || inner == 0) {
        return Matrix(left.size(), std::vector<double>(rightColumns, 0.0));
    }

    const std::size_t required = left.size() > inner
        ? (left.size() > rightColumns ? left.size() : rightColumns)
        : (inner > rightColumns ? inner : rightColumns);
    std::size_t size = 1;
    while (size < required) {
        if (size > std::numeric_limits<std::size_t>::max() / 2) {
            throw std::length_error("Matrix is too large for Strassen padding.");
        }
        size *= 2;
    }
    if (size > std::numeric_limits<std::size_t>::max() / size) {
        throw std::length_error("Matrix is too large for Strassen storage.");
    }

    FlatMatrix paddedLeft(size * size, 0.0);
    FlatMatrix paddedRight(size * size, 0.0);
    for (std::size_t row = 0; row < left.size(); ++row) {
        for (std::size_t column = 0; column < inner; ++column) {
            paddedLeft[row * size + column] = left[row][column];
        }
    }
    for (std::size_t row = 0; row < right.size(); ++row) {
        for (std::size_t column = 0; column < rightColumns; ++column) {
            paddedRight[row * size + column] = right[row][column];
        }
    }
    const FlatMatrix full = strassen(paddedLeft, paddedRight, size);
    Matrix product(left.size(), std::vector<double>(rightColumns));
    for (std::size_t row = 0; row < left.size(); ++row) {
        for (std::size_t column = 0; column < rightColumns; ++column) {
            product[row][column] = full[row * size + column];
        }
    }
    return product;
}

} // namespace project31::algorithms
