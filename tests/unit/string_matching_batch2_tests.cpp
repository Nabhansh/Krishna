#include "algorithms/string_matching/FiniteAutomatonStringMatching.hpp"
#include "algorithms/string_matching/KnuthMorrisPratt.hpp"
#include "algorithms/string_matching/NaiveStringMatching.hpp"
#include "algorithms/string_matching/RabinKarp.hpp"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace project31::algorithms {
namespace {

void expectAllMatchers(
    const std::string& text,
    const std::string& pattern,
    const std::vector<std::size_t>& expected
) {
    EXPECT_EQ(naiveStringMatch(text, pattern), expected);
    EXPECT_EQ(rabinKarpStringMatch(text, pattern), expected);
    EXPECT_EQ(finiteAutomatonStringMatch(text, pattern), expected);
    EXPECT_EQ(knuthMorrisPrattStringMatch(text, pattern), expected);
}

} // namespace

TEST(FiniteAutomatonStringMatchingTests, FindsBeginningMiddleAndEndOccurrences) {
    expectAllMatchers("FLT-217/FLT-817/FLT-217", "FLT-217", {0, 16});
}

TEST(FiniteAutomatonStringMatchingTests, FindsOverlappingAndPeriodicMatches) {
    expectAllMatchers("AAAAA", "AAA", {0, 1, 2});
    expectAllMatchers("ABABABABA", "ABABA", {0, 2, 4});
}

TEST(FiniteAutomatonStringMatchingTests, HandlesEmptyNoMatchAndLongPatterns) {
    expectAllMatchers("", "", {0});
    expectAllMatchers("event", "", {0, 1, 2, 3, 4, 5});
    expectAllMatchers("", "event", {});
    expectAllMatchers("log", "longer", {});
    expectAllMatchers("checkpoint", "security", {});
}

TEST(FiniteAutomatonStringMatchingTests, HandlesSingleCharacterAndHighBytes) {
    expectAllMatchers("BANANA", "A", {1, 3, 5});

    std::string highBytes;
    highBytes.push_back(static_cast<char>(0x80));
    highBytes.push_back(static_cast<char>(0xff));
    highBytes.push_back(static_cast<char>(0x80));
    highBytes.push_back(static_cast<char>(0xff));
    const std::string highBytePattern{
        static_cast<char>(0x80),
        static_cast<char>(0xff)
    };
    expectAllMatchers(highBytes, highBytePattern, {0, 2});
}

TEST(KnuthMorrisPrattStringMatchingTests, HandlesRepeatedAndAdversarialPatterns) {
    const std::string text(1024, 'A');
    const std::string pattern(64, 'A');
    std::vector<std::size_t> expected;
    for (std::size_t index = 0; index <= text.size() - pattern.size(); ++index) {
        expected.push_back(index);
    }
    expectAllMatchers(text, pattern, expected);

    const std::string nearMatchText = std::string(1024, 'A') + "B";
    const std::string nearMatchPattern = std::string(64, 'A') + "B";
    expectAllMatchers(nearMatchText, nearMatchPattern, {960});
}

TEST(StringMatchingBatch2Tests, DoesNotModifyInputsAndIsDeterministic) {
    const std::string text = "CHECKPOINT ALERT CHECKPOINT ALERT";
    const std::string pattern = "ALERT";
    const std::string originalText = text;
    const std::string originalPattern = pattern;

    const auto automatonFirst = finiteAutomatonStringMatch(text, pattern);
    const auto kmpFirst = knuthMorrisPrattStringMatch(text, pattern);
    EXPECT_EQ(finiteAutomatonStringMatch(text, pattern), automatonFirst);
    EXPECT_EQ(knuthMorrisPrattStringMatch(text, pattern), kmpFirst);
    EXPECT_EQ(automatonFirst, (std::vector<std::size_t>{11, 28}));
    EXPECT_EQ(kmpFirst, automatonFirst);
    EXPECT_EQ(text, originalText);
    EXPECT_EQ(pattern, originalPattern);
}

TEST(StringMatchingBatch2Tests, AllFourMatchersAgreeOnGeneratedSmallInputs) {
    std::uint32_t state = 0x5eed1234U;
    const auto nextSymbol = [&state]() {
        state = state * 1'664'525U + 1'013'904'223U;
        return static_cast<char>('A' + ((state >> 24U) % 3U));
    };

    for (std::size_t trial = 0; trial < 500; ++trial) {
        const std::size_t textLength = trial % 65U;
        const std::size_t patternLength = (trial * 13U) % 23U;
        std::string text(textLength, 'A');
        std::string pattern(patternLength, 'A');
        for (char& symbol : text) {
            symbol = nextSymbol();
        }
        for (char& symbol : pattern) {
            symbol = nextSymbol();
        }
        if (trial % 4U == 0 && !pattern.empty() &&
            pattern.size() <= text.size()) {
            const std::size_t start =
                (trial * 7U) % (text.size() - pattern.size() + 1U);
            text.replace(start, pattern.size(), pattern);
        }

        const auto expected = naiveStringMatch(text, pattern);
        EXPECT_EQ(rabinKarpStringMatch(text, pattern), expected)
            << "Rabin-Karp trial " << trial;
        EXPECT_EQ(finiteAutomatonStringMatch(text, pattern), expected)
            << "Finite automaton trial " << trial;
        EXPECT_EQ(knuthMorrisPrattStringMatch(text, pattern), expected)
            << "KMP trial " << trial;
    }
}

} // namespace project31::algorithms
