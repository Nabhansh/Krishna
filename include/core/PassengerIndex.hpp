#pragma once

#include "core/Passenger.hpp"

#include <map>
#include <string>
#include <vector>

namespace project31 {

enum class PassengerIdSort {
    Merge,
    Quick
};

class PassengerIndex {
public:
    void add(const Passenger& passenger);

    [[nodiscard]] bool containsId(const std::string& passengerId) const;
    [[nodiscard]] const std::vector<std::string>& passengerIdsSorted() const noexcept;
    [[nodiscard]] std::vector<std::string> passengerIdsSortedWith(
        PassengerIdSort algorithm
    ) const;
    [[nodiscard]] std::vector<std::string> idsForPriority(int priority) const;

private:
    std::vector<std::string> passengerIdsSorted_;
    std::map<int, std::vector<std::string>> idsByPriority_;
};

} // namespace project31
