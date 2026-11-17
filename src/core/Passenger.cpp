#include "core/Passenger.hpp"

#include <cctype>
#include <stdexcept>
#include <utility>

namespace project31 {
namespace {

bool isBlank(const std::string& value) {
    for (const unsigned char character : value) {
        if (!std::isspace(character)) {
            return false;
        }
    }
    return true;
}

} // namespace

Passenger::Passenger(
    std::string id,
    std::string name,
    int screeningDurationMinutes,
    int priority
)
    : id_(std::move(id)),
      name_(std::move(name)),
      screeningDurationMinutes_(screeningDurationMinutes),
      priority_(priority) {}

const std::string& Passenger::id() const noexcept {
    return id_;
}

const std::string& Passenger::name() const noexcept {
    return name_;
}

int Passenger::screeningDurationMinutes() const noexcept {
    return screeningDurationMinutes_;
}

int Passenger::priority() const noexcept {
    return priority_;
}

PassengerStatus Passenger::status() const noexcept {
    return status_;
}

std::vector<std::string> Passenger::validationErrors() const {
    std::vector<std::string> errors;
    if (isBlank(id_)) {
        errors.emplace_back("Passenger ID must not be empty.");
    }
    if (isBlank(name_)) {
        errors.emplace_back("Passenger name must not be empty.");
    }
    if (screeningDurationMinutes_ <= 0) {
        errors.emplace_back("Screening duration must be greater than zero.");
    }
    return errors;
}

void Passenger::markQueued() {
    if (status_ != PassengerStatus::Registered) {
        throw std::logic_error("Only a registered passenger can join a security queue.");
    }
    status_ = PassengerStatus::Queued;
}

void Passenger::markScreened() {
    if (status_ != PassengerStatus::Queued) {
        throw std::logic_error("Only a queued passenger can be screened.");
    }
    status_ = PassengerStatus::Screened;
}

} // namespace project31
