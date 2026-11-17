#pragma once

#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace project31::algorithms {

class FiniteAutomatonStringMatcher {
public:
    explicit FiniteAutomatonStringMatcher(const std::string& pattern);

    [[nodiscard]] std::vector<std::size_t> findAll(
        const std::string& text
    ) const;

private:
    std::string pattern_;
    std::vector<std::array<std::size_t, 256>> transitions_;
};

[[nodiscard]] std::vector<std::size_t> finiteAutomatonStringMatch(
    const std::string& text,
    const std::string& pattern
);

} // namespace project31::algorithms
