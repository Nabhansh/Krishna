#include "algorithms/divide_conquer/FastExponentiation.hpp"

#include <cmath>
#include <stdexcept>

namespace project31::algorithms {
namespace {

double checkedMultiply(double left, double right) {
    const double result = left * right;
    if (!std::isfinite(result)) {
        throw std::overflow_error("Exponentiation result exceeds floating-point range.");
    }
    return result;
}

} // namespace

double fastExponentiation(double base, std::int64_t exponent) {
    if (!std::isfinite(base)) {
        throw std::invalid_argument("Exponentiation base must be finite.");
    }
    if (base == 0.0 && exponent < 0) {
        throw std::invalid_argument("Zero cannot be raised to a negative exponent.");
    }
    if (exponent == 0) {
        return 1.0;
    }

    const bool reciprocal = exponent < 0;
    std::uint64_t power = reciprocal
        ? static_cast<std::uint64_t>(-(exponent + 1)) + 1U
        : static_cast<std::uint64_t>(exponent);
    double result = 1.0;
    double factor = reciprocal ? 1.0 / base : base;
    if (!std::isfinite(factor)) {
        throw std::overflow_error("Exponentiation result exceeds floating-point range.");
    }
    while (power != 0) {
        if ((power & 1U) != 0) {
            result = checkedMultiply(result, factor);
        }
        power >>= 1U;
        if (power != 0) {
            factor = checkedMultiply(factor, factor);
        }
    }
    return result;
}

} // namespace project31::algorithms
