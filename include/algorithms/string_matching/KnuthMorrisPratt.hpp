#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace project31::algorithms {

class KnuthMorrisPrattStringMatcher {
public:
    explicit KnuthMorrisPrattStringMatcher(const std::string& pattern);

    [[nodiscard]] std::vector<std::size_t> findAll(
        const std::string& text
    ) const;

private:
    std::string pattern_;
    std::vector<std::size_t> prefixLengths_;
};

[[nodiscard]] std::vector<std::size_t> knuthMorrisPrattStringMatch(
    const std::string& text,
    const std::string& pattern
);

} // namespace project31::algorithms
