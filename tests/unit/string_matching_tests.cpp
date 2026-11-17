#include "algorithms/string_matching/NaiveStringMatching.hpp"
#include "algorithms/string_matching/RabinKarp.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace project31::algorithms {
namespace {

void expectBoth(
    const std::string& text,
    const std::string& pattern,
    const std::vector<std::size_t>& expected
) {
    EXPECT_EQ(naiveStringMatch(text, pattern), expected);
    EXPECT_EQ(rabinKarpStringMatch(text, pattern), expected);
}

std::uint64_t testHash(const std::string& value) {
    constexpr std::uint64_t modulus = 1'000'000'007U;
    constexpr std::uint64_t base = 257U;
    std::uint64_t hash = 0;
    for (unsigned char byte : value) {
        hash = (hash * base + byte + 1U) % modulus;
    }
    return hash;
}

} // namespace

TEST(NaiveStringMatchingTests, FindsMatchesAtBeginningMiddleAndEnd) {
    expectBoth("ABxxABxxAB", "AB", {0, 4, 8});
}

TEST(NaiveStringMatchingTests, FindsOverlappingOccurrences) {
    expectBoth("aaaaa", "aaa", {0, 1, 2});
}

TEST(NaiveStringMatchingTests, HandlesNoMatchAndPatternLongerThanText) {
    expectBoth("airport", "gate", {});
    expectBoth("gate", "airport", {});
}

TEST(NaiveStringMatchingTests, DefinesEmptyTextAndEmptyPatternBoundaries) {
    expectBoth("", "", {0});
    expectBoth("", "marker", {});
    expectBoth("log", "", {0, 1, 2, 3});
}

TEST(NaiveStringMatchingTests, MatchesBytesIncludingEmbeddedNulls) {
    const std::string text("a\0b\0b", 5);
    const std::string pattern("\0b", 2);
    expectBoth(text, pattern, {1, 3});
}

TEST(RabinKarpStringMatchingTests, FindsRepeatedAndPeriodicOccurrences) {
    expectBoth("abababababa", "ababa", {0, 2, 4, 6});
    expectBoth("xxxxxxxx", "xxx", {0, 1, 2, 3, 4, 5});
}

TEST(RabinKarpStringMatchingTests, VerifiesByteEqualityAfterAHashCollision) {
    const std::string collidingWindow("\x8f\x57\x67\x5e", 4);
    const std::string pattern("\x54\x6e\x20\xc2", 4);
    ASSERT_EQ(testHash(collidingWindow), testHash(pattern));

    const std::string text = collidingWindow + "!" + pattern;
    EXPECT_EQ(rabinKarpStringMatch(text, pattern), (std::vector<std::size_t>{5}));
    EXPECT_EQ(naiveStringMatch(text, pattern), (std::vector<std::size_t>{5}));
}

TEST(RabinKarpStringMatchingTests, MatchesNaiveOnDeterministicGeneratedInputs) {
    std::uint32_t state = 0x31a7U;
    const auto nextByte = [&state]() {
        state = state * 1'664'525U + 1'013'904'223U;
        return static_cast<char>((state >> 24U) & 0xffU);
    };

    for (std::size_t trial = 0; trial < 400; ++trial) {
        std::string text;
        std::string pattern;
        const std::size_t textLength = trial % 53;
        const std::size_t patternLength = (trial * 7U) % 17;
        text.reserve(textLength);
        pattern.reserve(patternLength);
        for (std::size_t index = 0; index < textLength; ++index) {
            text.push_back(nextByte());
        }
        for (std::size_t index = 0; index < patternLength; ++index) {
            pattern.push_back(nextByte());
        }
        if (trial % 3U == 0 && patternLength <= textLength && !pattern.empty()) {
            const std::size_t start = (trial * 11U) % (textLength - patternLength + 1);
            text.replace(start, patternLength, pattern);
        }
        EXPECT_EQ(
            rabinKarpStringMatch(text, pattern),
            naiveStringMatch(text, pattern)
        ) << "Generated trial " << trial;
    }
}

} // namespace project31::algorithms
