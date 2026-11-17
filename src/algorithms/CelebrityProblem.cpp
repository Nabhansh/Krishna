#include "algorithms/divide_conquer/CelebrityProblem.hpp"

#include <stdexcept>
#include <vector>

namespace project31::algorithms {

std::optional<std::size_t> findCelebrity(const std::vector<std::vector<int>>& knows) {
    const std::size_t count = knows.size();
    for (const std::vector<int>& row : knows) {
        if (row.size() != count) {
            throw std::invalid_argument("Celebrity relation matrix must be square.");
        }
        for (int relation : row) {
            if (relation != 0 && relation != 1) {
                throw std::invalid_argument("Celebrity relation values must be 0 or 1.");
            }
        }
    }
    if (count == 0) {
        return std::nullopt;
    }

    std::size_t candidate = 0;
    for (std::size_t person = 1; person < count; ++person) {
        if (knows[candidate][person] == 1) {
            candidate = person;
        }
    }

    for (std::size_t person = 0; person < count; ++person) {
        if (person != candidate &&
            (knows[candidate][person] != 0 || knows[person][candidate] != 1)) {
            return std::nullopt;
        }
    }
    return candidate;
}

} // namespace project31::algorithms
