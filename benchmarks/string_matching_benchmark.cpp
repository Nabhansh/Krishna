#include "algorithms/string_matching/FiniteAutomatonStringMatching.hpp"
#include "algorithms/string_matching/KnuthMorrisPratt.hpp"
#include "algorithms/string_matching/NaiveStringMatching.hpp"
#include "algorithms/string_matching/RabinKarp.hpp"

#include <chrono>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace {

template <typename Search>
double measureMilliseconds(
    Search search,
    const std::string& text,
    const std::string& pattern,
    const std::vector<std::size_t>& expected
) {
    const auto start = std::chrono::steady_clock::now();
    const std::vector<std::size_t> result = search(text, pattern);
    const auto finish = std::chrono::steady_clock::now();
    if (result != expected) {
        return -1.0;
    }
    return std::chrono::duration<double, std::milli>(finish - start).count();
}

template <typename PreparedMatcher>
double measurePreparedSearch(
    const PreparedMatcher& matcher,
    const std::string& text,
    const std::vector<std::size_t>& expected
) {
    const auto start = std::chrono::steady_clock::now();
    const std::vector<std::size_t> result = matcher.findAll(text);
    const auto finish = std::chrono::steady_clock::now();
    if (result != expected) {
        return -1.0;
    }
    return std::chrono::duration<double, std::milli>(finish - start).count();
}

std::string periodicText(std::size_t length) {
    std::string text(length, 'A');
    if (!text.empty()) {
        text.back() = 'B';
    }
    return text;
}

} // namespace

int main() {
    using project31::algorithms::naiveStringMatch;
    using project31::algorithms::rabinKarpStringMatch;
    using project31::algorithms::finiteAutomatonStringMatch;
    using project31::algorithms::knuthMorrisPrattStringMatch;
    const std::vector<std::pair<std::string, std::size_t>> datasetSizes{
        {"small", 100},
        {"medium", 5'000},
        {"large", 50'000},
        {"periodic-adversarial", 20'000}
    };

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Airport log matching benchmark (milliseconds)\n"
                 "Dataset                 Bytes      Naive   Rabin-Karp"
                 "  DFA-prep  DFA-search  KMP-prep  KMP-search\n";
    for (const auto& [label, size] : datasetSizes) {
        const bool adversarial = label == "periodic-adversarial";
        std::string text = adversarial ? periodicText(size) : std::string(size, 'x');
        std::string pattern = adversarial ? std::string(127, 'A') + "B" : "flight";
        if (!adversarial && size >= 20) {
            text.replace(size / 2, pattern.size(), pattern);
        }

        const std::vector<std::size_t> naiveMatches =
            naiveStringMatch(text, pattern);
        const std::vector<std::size_t> rabinKarpMatches =
            rabinKarpStringMatch(text, pattern);
        const std::vector<std::size_t> automatonMatches =
            finiteAutomatonStringMatch(text, pattern);
        const std::vector<std::size_t> kmpMatches =
            knuthMorrisPrattStringMatch(text, pattern);
        if (naiveMatches != rabinKarpMatches ||
            naiveMatches != automatonMatches ||
            naiveMatches != kmpMatches) {
            std::cerr << "Match positions differ for " << label << ".\n";
            return 1;
        }

        const double naiveMilliseconds =
            measureMilliseconds(
                naiveStringMatch,
                text,
                pattern,
                naiveMatches
            );
        const double rabinKarpMilliseconds =
            measureMilliseconds(
                rabinKarpStringMatch,
                text,
                pattern,
                naiveMatches
            );

        std::optional<project31::algorithms::FiniteAutomatonStringMatcher>
            automaton;
        auto start = std::chrono::steady_clock::now();
        automaton.emplace(pattern);
        auto finish = std::chrono::steady_clock::now();
        const double automatonPreparationMilliseconds =
            std::chrono::duration<double, std::milli>(finish - start).count();
        const double automatonSearchMilliseconds =
            measurePreparedSearch(*automaton, text, naiveMatches);

        std::optional<project31::algorithms::KnuthMorrisPrattStringMatcher> kmp;
        start = std::chrono::steady_clock::now();
        kmp.emplace(pattern);
        finish = std::chrono::steady_clock::now();
        const double kmpPreparationMilliseconds =
            std::chrono::duration<double, std::milli>(finish - start).count();
        const double kmpSearchMilliseconds =
            measurePreparedSearch(*kmp, text, naiveMatches);

        if (naiveMilliseconds < 0.0 || rabinKarpMilliseconds < 0.0 ||
            automatonSearchMilliseconds < 0.0 || kmpSearchMilliseconds < 0.0) {
            std::cerr << "Matcher validation failed for " << label << ".\n";
            return 1;
        }
        std::cout << std::left << std::setw(24) << label
                  << std::right << std::setw(8) << size
                  << std::setw(12) << naiveMilliseconds
                  << std::setw(13) << rabinKarpMilliseconds
                  << std::setw(11) << automatonPreparationMilliseconds
                  << std::setw(13) << automatonSearchMilliseconds
                  << std::setw(11) << kmpPreparationMilliseconds
                  << std::setw(13) << kmpSearchMilliseconds << '\n';
    }
    return 0;
}
