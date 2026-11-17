#include "algorithms/divide_conquer/CelebrityProblem.hpp"
#include "algorithms/divide_conquer/ClassicalMatrixMultiplication.hpp"
#include "algorithms/divide_conquer/FastExponentiation.hpp"
#include "algorithms/divide_conquer/MaximumSubarray.hpp"
#include "algorithms/divide_conquer/StrassenMatrixMultiplication.hpp"

#include <iostream>

int main() {
    const auto peak = project31::algorithms::maximumSubarray({-2, 4, -1, 3, -5});
    std::cout << "Arrival peak change: " << peak->sum << '\n';
    std::cout << "Screening capacity scale: "
              << project31::algorithms::fastExponentiation(2.0, 5) << '\n';
    const auto celebrity =
        project31::algorithms::findCelebrity({{0, 1, 1}, {0, 0, 0}, {0, 1, 0}});
    std::cout << "Celebrity candidate: " << *celebrity << '\n';

    const project31::algorithms::Matrix left{{1, 2}, {3, 4}};
    const project31::algorithms::Matrix right{{5, 6}, {7, 8}};
    const auto classical = project31::algorithms::classicalMatrixMultiply(left, right);
    const auto strassen = project31::algorithms::strassenMatrixMultiply(left, right);
    std::cout << "Classical/Strassen sample result: "
              << classical[0][0] << '/' << strassen[0][0] << '\n';
    return 0;
}
