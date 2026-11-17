#pragma once

#include <string>
#include <vector>

namespace project31 {

enum class PassengerStatus {
    Registered,
    Queued,
    Screened
};

class Passenger {
public:
    Passenger(
        std::string id,
        std::string name,
        int screeningDurationMinutes,
        int priority = 0
    );

    [[nodiscard]] const std::string& id() const noexcept;
    [[nodiscard]] const std::string& name() const noexcept;
    [[nodiscard]] int screeningDurationMinutes() const noexcept;
    [[nodiscard]] int priority() const noexcept;
    [[nodiscard]] PassengerStatus status() const noexcept;
    [[nodiscard]] std::vector<std::string> validationErrors() const;

    void markQueued();
    void markScreened();

private:
    std::string id_;
    std::string name_;
    int screeningDurationMinutes_;
    int priority_;
    PassengerStatus status_{PassengerStatus::Registered};
};

} // namespace project31
