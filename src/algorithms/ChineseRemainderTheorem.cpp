#include "algorithms/number_theory/ChineseRemainderTheorem.hpp"

#include <cstdint>
#include <limits>
#include <numeric>
#include <stdexcept>

namespace project31::algorithms {
namespace {

using Unsigned = std::uint64_t;

Unsigned addModulo(Unsigned left, Unsigned right, Unsigned modulus) {
    if (left >= modulus - right) {
        return left - (modulus - right);
    }
    return left + right;
}

Unsigned subtractModulo(Unsigned left, Unsigned right, Unsigned modulus) {
    return left >= right ? left - right : modulus - (right - left);
}

Unsigned multiplyModulo(Unsigned left, Unsigned right, Unsigned modulus) {
    left %= modulus;
    right %= modulus;
    Unsigned result = 0;
    while (right != 0) {
        if ((right & 1U) != 0) {
            result = addModulo(result, left, modulus);
        }
        right >>= 1U;
        if (right != 0) {
            left = addModulo(left, left, modulus);
        }
    }
    return result;
}

std::int64_t normalize(std::int64_t residue, std::int64_t modulus) {
    std::int64_t result = residue % modulus;
    if (result < 0) {
        result += modulus;
    }
    return result;
}

std::int64_t modularInverse(std::int64_t value, std::int64_t modulus) {
    if (modulus == 1) {
        return 0;
    }

    std::int64_t oldRemainder = modulus;
    std::int64_t remainder = value;
    Unsigned oldCoefficient = 0;
    Unsigned coefficient = 1;
    const Unsigned unsignedModulus = static_cast<Unsigned>(modulus);

    while (remainder != 0) {
        const std::int64_t quotient = oldRemainder / remainder;
        const std::int64_t nextRemainder = oldRemainder % remainder;
        const Unsigned scaledCoefficient = multiplyModulo(
            static_cast<Unsigned>(quotient),
            coefficient,
            unsignedModulus
        );
        const Unsigned nextCoefficient = subtractModulo(
            oldCoefficient,
            scaledCoefficient,
            unsignedModulus
        );
        oldRemainder = remainder;
        remainder = nextRemainder;
        oldCoefficient = coefficient;
        coefficient = nextCoefficient;
    }
    if (oldRemainder != 1) {
        throw std::invalid_argument("CRT moduli must be pairwise coprime.");
    }
    return static_cast<std::int64_t>(oldCoefficient);
}

} // namespace

ChineseRemainderResult solveChineseRemainderTheorem(
    const std::vector<std::int64_t>& residues,
    const std::vector<std::int64_t>& moduli
) {
    if (residues.size() != moduli.size()) {
        throw std::invalid_argument("CRT residues and moduli must have equal lengths.");
    }

    std::int64_t solution = 0;
    std::int64_t combinedModulus = 1;
    for (std::size_t index = 0; index < moduli.size(); ++index) {
        const std::int64_t modulus = moduli[index];
        if (modulus <= 0) {
            throw std::invalid_argument("CRT moduli must be positive.");
        }
        if (std::gcd(combinedModulus, modulus) != 1) {
            throw std::invalid_argument("CRT moduli must be pairwise coprime.");
        }
        if (combinedModulus >
            std::numeric_limits<std::int64_t>::max() / modulus) {
            throw std::overflow_error("CRT combined modulus exceeds int64 range.");
        }

        const std::int64_t normalizedResidue = normalize(residues[index], modulus);
        const std::int64_t currentModulo = normalize(solution, modulus);
        const std::int64_t difference = normalizedResidue >= currentModulo
            ? normalizedResidue - currentModulo
            : modulus - (currentModulo - normalizedResidue);
        const std::int64_t coefficient = normalize(combinedModulus, modulus);
        const std::int64_t inverse = modularInverse(coefficient, modulus);
        const Unsigned multiplier = multiplyModulo(
            static_cast<Unsigned>(difference),
            static_cast<Unsigned>(inverse),
            static_cast<Unsigned>(modulus)
        );
        const std::int64_t scaledStep = static_cast<std::int64_t>(multiplier);
        if (scaledStep != 0 &&
            combinedModulus >
                std::numeric_limits<std::int64_t>::max() / scaledStep) {
            throw std::overflow_error("CRT solution update exceeds int64 range.");
        }
        const std::int64_t offset = combinedModulus * scaledStep;
        if (solution > std::numeric_limits<std::int64_t>::max() - offset) {
            throw std::overflow_error("CRT solution update exceeds int64 range.");
        }
        solution += offset;
        combinedModulus *= modulus;
    }
    return {solution, combinedModulus};
}

} // namespace project31::algorithms
