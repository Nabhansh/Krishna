#include "simulator/AirportOperations.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace {

void printMatches(const char* label, const std::vector<std::size_t>& matches) {
    std::cout << label << " match offsets:";
    for (std::size_t match : matches) {
        std::cout << ' ' << match;
    }
    std::cout << '\n';
}

} // namespace

int main() {
    const std::string log =
        "ARRIVAL FLT-217 gate; FLT-817 delayed; FLT-217 departed";
    const std::string flightCode = "FLT-217";
    const auto naive =
        project31::AirportOperations::findFlightCodesInLogNaive(log, flightCode);
    const auto rabinKarp =
        project31::AirportOperations::findFlightCodesInLogRabinKarp(log, flightCode);
    const auto finiteAutomaton =
        project31::AirportOperations::findFlightCodesInLogFiniteAutomaton(
            log, flightCode
        );
    const auto kmp =
        project31::AirportOperations::findFlightCodesInLogKnuthMorrisPratt(
            log, flightCode
        );
    if (naive != rabinKarp || naive != finiteAutomaton || naive != kmp) {
        std::cerr << "String matching implementations disagree.\n";
        return 1;
    }
    printMatches("Naive", naive);
    printMatches("Rabin-Karp", rabinKarp);
    printMatches("Finite automaton", finiteAutomaton);
    printMatches("KMP", kmp);
    return 0;
}
