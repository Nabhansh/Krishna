#pragma once

#include "core/Passenger.hpp"
#include "core/SecurityOfficer.hpp"

#include <cstdint>
#include <deque>
#include <optional>
#include <string>

namespace project31 {

class SecurityLane {
public:
    SecurityLane(std::string id, SecurityOfficer officer);

    [[nodiscard]] const std::string& id() const noexcept;
    [[nodiscard]] const SecurityOfficer& officer() const noexcept;
    [[nodiscard]] const std::deque<Passenger>& queue() const noexcept;
    [[nodiscard]] std::int64_t workloadMinutes() const noexcept;

    void enqueue(Passenger passenger);
    [[nodiscard]] std::optional<Passenger> screenNext();

private:
    std::string id_;
    SecurityOfficer officer_;
    std::deque<Passenger> queue_;
};

} // namespace project31
