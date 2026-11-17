#include "algorithms/string_matching/FiniteAutomatonStringMatching.hpp"

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace project31::algorithms {
namespace {

std::vector<std::size_t> emptyPatternMatches(std::size_t textLength) {
    std::vector<std::size_t> matches;
    matches.reserve(textLength + 1);
    for (std::size_t boundary = 0; boundary <= textLength; ++boundary) {
        matches.push_back(boundary);
    }
    return matches;
}

} // namespace

FiniteAutomatonStringMatcher::FiniteAutomatonStringMatcher(
    const std::string& pattern
) : pattern_(pattern) {
    if (pattern_.empty()) {
        return;
    }
    if (pattern_.size() == std::numeric_limits<std::size_t>::max()) {
        throw std::length_error("Finite automaton pattern is too large.");
    }
    const std::size_t stateCount = pattern_.size() + 1;
    if (stateCount > transitions_.max_size()) {
        throw std::length_error("Finite automaton transition table is too large.");
    }
    transitions_.resize(stateCount);

    // State q represents the length of the longest matched pattern prefix.
    transitions_[0].fill(0);
    transitions_[0][static_cast<unsigned char>(pattern_[0])] = 1;

    std::size_t fallbackState = 0;
    for (std::size_t state = 1; state < pattern_.size(); ++state) {
        transitions_[state] = transitions_[fallbackState];
        const auto nextByte = static_cast<unsigned char>(pattern_[state]);
        transitions_[state][nextByte] = state + 1;
        fallbackState = transitions_[fallbackState][nextByte];
    }

    // From an accepting state, reuse transitions from its longest border.
    transitions_[pattern_.size()] = transitions_[fallbackState];
}

std::vector<std::size_t> FiniteAutomatonStringMatcher::findAll(
    const std::string& text
) const {
    if (pattern_.empty()) {
        return emptyPatternMatches(text.size());
    }
    std::vector<std::size_t> matches;
    if (pattern_.size() > text.size()) {
        return matches;
    }

    std::size_t state = 0;
    for (std::size_t index = 0; index < text.size(); ++index) {
        const auto byte = static_cast<unsigned char>(text[index]);
        state = transitions_[state][byte];
        if (state == pattern_.size()) {
            matches.push_back(index + 1 - pattern_.size());
        }
    }
    return matches;
}

std::vector<std::size_t> finiteAutomatonStringMatch(
    const std::string& text,
    const std::string& pattern
) {
    return FiniteAutomatonStringMatcher(pattern).findAll(text);
}

} // namespace project31::algorithms
