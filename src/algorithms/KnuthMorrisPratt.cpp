#include "algorithms/string_matching/KnuthMorrisPratt.hpp"

#include <cstddef>
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

KnuthMorrisPrattStringMatcher::KnuthMorrisPrattStringMatcher(
    const std::string& pattern
) : pattern_(pattern), prefixLengths_(pattern_.size(), 0) {
    std::size_t matchedPrefixLength = 0;
    for (std::size_t index = 1; index < pattern_.size(); ++index) {
        const auto currentByte = static_cast<unsigned char>(pattern_[index]);
        while (matchedPrefixLength > 0 &&
               currentByte != static_cast<unsigned char>(
                   pattern_[matchedPrefixLength]
               )) {
            matchedPrefixLength = prefixLengths_[matchedPrefixLength - 1];
        }
        if (currentByte ==
            static_cast<unsigned char>(pattern_[matchedPrefixLength])) {
            ++matchedPrefixLength;
        }
        prefixLengths_[index] = matchedPrefixLength;
    }
}

std::vector<std::size_t> KnuthMorrisPrattStringMatcher::findAll(
    const std::string& text
) const {
    if (pattern_.empty()) {
        return emptyPatternMatches(text.size());
    }
    std::vector<std::size_t> matches;
    if (pattern_.size() > text.size()) {
        return matches;
    }

    std::size_t patternIndex = 0;
    for (std::size_t textIndex = 0; textIndex < text.size();) {
        const auto textByte = static_cast<unsigned char>(text[textIndex]);
        const auto patternByte =
            static_cast<unsigned char>(pattern_[patternIndex]);
        if (textByte == patternByte) {
            ++textIndex;
            ++patternIndex;
            if (patternIndex == pattern_.size()) {
                matches.push_back(textIndex - patternIndex);
                patternIndex = prefixLengths_[patternIndex - 1];
            }
        } else if (patternIndex > 0) {
            patternIndex = prefixLengths_[patternIndex - 1];
        } else {
            ++textIndex;
        }
    }
    return matches;
}

std::vector<std::size_t> knuthMorrisPrattStringMatch(
    const std::string& text,
    const std::string& pattern
) {
    return KnuthMorrisPrattStringMatcher(pattern).findAll(text);
}

} // namespace project31::algorithms
