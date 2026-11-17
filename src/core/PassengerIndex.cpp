#include "core/PassengerIndex.hpp"

#include "algorithms/searching/BinarySearch.hpp"
#include "algorithms/sorting/MergeSort.hpp"
#include "algorithms/sorting/QuickSort.hpp"

#include <cstddef>
#include <stdexcept>

namespace project31 {

void PassengerIndex::add(const Passenger& passenger) {
    if (containsId(passenger.id())) {
        throw std::invalid_argument("Passenger ID is already indexed: " + passenger.id());
    }

    const std::size_t insertionIndex =
        algorithms::lowerBound(passengerIdsSorted_, passenger.id());
    passengerIdsSorted_.insert(
        passengerIdsSorted_.begin() + static_cast<std::ptrdiff_t>(insertionIndex),
        passenger.id()
    );
    idsByPriority_[passenger.priority()].push_back(passenger.id());
}

bool PassengerIndex::containsId(const std::string& passengerId) const {
    return algorithms::iterativeBinarySearch(passengerIdsSorted_, passengerId).has_value();
}

const std::vector<std::string>& PassengerIndex::passengerIdsSorted() const noexcept {
    return passengerIdsSorted_;
}

std::vector<std::string> PassengerIndex::passengerIdsSortedWith(
    PassengerIdSort algorithm
) const {
    std::vector<std::string> ids = passengerIdsSorted_;
    if (algorithm == PassengerIdSort::Merge) {
        algorithms::mergeSort(ids);
    } else {
        algorithms::quickSort(ids);
    }
    return ids;
}

std::vector<std::string> PassengerIndex::idsForPriority(int priority) const {
    const auto found = idsByPriority_.find(priority);
    if (found == idsByPriority_.end()) {
        return {};
    }
    std::vector<std::string> ids = found->second;
    algorithms::mergeSort(ids);
    return ids;
}

} // namespace project31
