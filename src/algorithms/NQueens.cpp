#include "algorithms/backtracking/NQueens.hpp"

#include <stdexcept>
#include <vector>

namespace project31::algorithms {
namespace {

void enumerateSolutions(
    std::size_t row,
    std::size_t boardSize,
    std::vector<std::size_t>& placement,
    std::vector<bool>& occupiedColumns,
    std::vector<bool>& occupiedDescendingDiagonals,
    std::vector<bool>& occupiedAscendingDiagonals,
    std::vector<std::vector<std::size_t>>& solutions
) {
    if (row == boardSize) {
        solutions.push_back(placement);
        return;
    }

    for (std::size_t column = 0; column < boardSize; ++column) {
        const std::size_t descendingDiagonal = row + column;
        const std::size_t ascendingDiagonal = row + (boardSize - 1 - column);
        if (occupiedColumns[column] ||
            occupiedDescendingDiagonals[descendingDiagonal] ||
            occupiedAscendingDiagonals[ascendingDiagonal]) {
            continue;
        }

        placement[row] = column;
        occupiedColumns[column] = true;
        occupiedDescendingDiagonals[descendingDiagonal] = true;
        occupiedAscendingDiagonals[ascendingDiagonal] = true;
        enumerateSolutions(
            row + 1,
            boardSize,
            placement,
            occupiedColumns,
            occupiedDescendingDiagonals,
            occupiedAscendingDiagonals,
            solutions
        );
        occupiedColumns[column] = false;
        occupiedDescendingDiagonals[descendingDiagonal] = false;
        occupiedAscendingDiagonals[ascendingDiagonal] = false;
    }
}

} // namespace

std::vector<std::vector<std::size_t>> solveNQueens(std::size_t boardSize) {
    if (boardSize == 0) {
        throw std::invalid_argument("N-Queens board size must be positive.");
    }
    if (boardSize > kMaximumEnumeratedQueens) {
        throw std::length_error(
            "N-Queens enumeration is limited to board sizes of 12."
        );
    }

    std::vector<std::vector<std::size_t>> solutions;
    std::vector<std::size_t> placement(boardSize);
    std::vector<bool> occupiedColumns(boardSize, false);
    std::vector<bool> occupiedDescendingDiagonals(2 * boardSize - 1, false);
    std::vector<bool> occupiedAscendingDiagonals(2 * boardSize - 1, false);
    enumerateSolutions(
        0,
        boardSize,
        placement,
        occupiedColumns,
        occupiedDescendingDiagonals,
        occupiedAscendingDiagonals,
        solutions
    );
    return solutions;
}

} // namespace project31::algorithms
