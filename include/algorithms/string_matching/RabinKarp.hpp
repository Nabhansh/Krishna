#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace project31::algorithms {

// Matches std::string bytes; an empty pattern matches all boundaries [0, text.size()].
[[nodiscard]] std::vector<std::size_t> rabinKarpStringMatch(
    const std::string& text,
    const std::string& pattern
);

} // namespace project31::algorithms
