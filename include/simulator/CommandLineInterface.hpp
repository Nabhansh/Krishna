#pragma once

#include "core/Airport.hpp"

#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>

namespace project31 {

class CommandLineInterface {
public:
    CommandLineInterface(std::istream& input, std::ostream& output);
    int run();

private:
    [[nodiscard]] std::optional<std::string> readLine(std::string_view prompt);
    [[nodiscard]] std::optional<int> readInteger(std::string_view prompt);
    void registerPassenger();
    void recordArrival();
    void screenPassenger();
    void showQueues() const;
    void findPassenger();
    void listPassengersByPriority();
    void listPassengerIdsSorted();

    std::istream& input_;
    std::ostream& output_;
    Airport airport_;
};

} // namespace project31
