#include "algorithms/string_matching/RabinKarp.hpp"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace project31::algorithms {
namespace {

constexpr std::uint64_t kHashModulus = 1'000'000'007U;
constexpr std::uint64_t kHashBase = 257U;

} // namespace

std::vector<std::size_t> rabinKarpStringMatch(
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

    std::uint64_t highestPower = 1;
    for (std::size_t index = 1; index < pattern.size(); ++index) {
        highestPower = (highestPower * kHashBase) % kHashModulus;
    }

    std::uint64_t patternHash = 0;
    std::uint64_t windowHash = 0;
    for (std::size_t index = 0; index < pattern.size(); ++index) {
        const auto patternByte = static_cast<unsigned char>(pattern[index]);
        const auto textByte = static_cast<unsigned char>(text[index]);
        patternHash = (patternHash * kHashBase + patternByte + 1U) % kHashModulus;
        windowHash = (windowHash * kHashBase + textByte + 1U) % kHashModulus;
    }

    const std::size_t lastStart = text.size() - pattern.size();
    for (std::size_t start = 0; start <= lastStart; ++start) {
        if (windowHash == patternHash &&
            text.compare(start, pattern.size(), pattern) == 0) {
            matches.push_back(start);
        }
        if (start == lastStart) {
            break;
        }
        const auto outgoingByte = static_cast<unsigned char>(text[start]);
        const std::uint64_t outgoingContribution =
            ((static_cast<std::uint64_t>(outgoingByte) + 1U) * highestPower) %
            kHashModulus;
        windowHash =
            ((windowHash + kHashModulus - outgoingContribution) * kHashBase +
             static_cast<unsigned char>(text[start + pattern.size()]) + 1U) %
            kHashModulus;
    }
    return matches;
}

} // namespace project31::algorithms
