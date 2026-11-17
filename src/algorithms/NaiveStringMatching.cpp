#include "algorithms/string_matching/NaiveStringMatching.hpp"

#include <cstddef>
#include <vector>

namespace project31::algorithms {

std::vector<std::size_t> naiveStringMatch(
    const std::string& text,
    const std::string& pattern
) {
    std::vector<std::size_t> matches;
    if (pattern.empty()) {
        matches.reserve(text.size() + 1);
        for (std::size_t boundary = 0; boundary <= text.size(); ++boundary) {
            matches.push_back(boundary);
        }
        return matches;
    }
    if (pattern.size() > text.size()) {
        return matches;
    }

    const std::size_t lastStart = text.size() - pattern.size();
    for (std::size_t start = 0; start <= lastStart; ++start) {
        std::size_t offset = 0;
        while (offset < pattern.size() && text[start + offset] == pattern[offset]) {
            ++offset;
        }
        if (offset == pattern.size()) {
            matches.push_back(start);
        }
    }
    return matches;
}

} // namespace project31::algorithms
