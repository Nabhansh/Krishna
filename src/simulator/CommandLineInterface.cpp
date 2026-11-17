#include "simulator/CommandLineInterface.hpp"

#include "core/SecurityLane.hpp"
#include "core/SecurityOfficer.hpp"

#include <charconv>
#include <iostream>
#include <string>
#include <vector>

namespace project31 {

CommandLineInterface::CommandLineInterface(std::istream& input, std::ostream& output)
    : input_(input), output_(output), airport_("Project31 Airport") {
    airport_.queueManager().addLane(
        SecurityLane("Lane-1", SecurityOfficer("Officer-1", "Officer One"))
    );
    airport_.queueManager().addLane(
        SecurityLane("Lane-2", SecurityOfficer("Officer-2", "Officer Two"))
    );
}

int CommandLineInterface::run() {
    output_ << "Project31 - " << airport_.name() << '\n';
    while (true) {
        output_ << "\n1. Register passenger\n"
                   "2. Record passenger arrival\n"
                   "3. Screen next passenger\n"
                   "4. Show security queues\n"
                   "5. Exit\n"
                   "6. Find passenger by ID\n"
                   "7. List passengers by priority\n"
                   "8. List passenger IDs using a sort algorithm\n";
        const std::optional<int> choice = readInteger("Choose an option: ");
        if (!choice) {
            output_ << "\nInput closed. Exiting.\n";
            return 0;
        }

        try {
            switch (*choice) {
            case 1:
                registerPassenger();
                break;
            case 2:
                recordArrival();
                break;
            case 3:
                screenPassenger();
                break;
            case 4:
                showQueues();
                break;
            case 5:
                output_ << "Goodbye.\n";
                return 0;
            case 6:
                findPassenger();
                break;
            case 7:
                listPassengersByPriority();
                break;
            case 8:
                listPassengerIdsSorted();
                break;
            default:
                output_ << "Choose a number from 1 to 8.\n";
                break;
            }
        } catch (const std::exception& error) {
            output_ << "Operation failed: " << error.what() << '\n';
        }
    }
}

std::optional<std::string> CommandLineInterface::readLine(std::string_view prompt) {
    output_ << prompt;
    std::string value;
    if (!std::getline(input_, value)) {
        return std::nullopt;
    }
    return value;
}

std::optional<int> CommandLineInterface::readInteger(std::string_view prompt) {
    while (true) {
        const std::optional<std::string> line = readLine(prompt);
        if (!line) {
            return std::nullopt;
        }
        int value{};
        const char* const begin = line->data();
        const char* const end = begin + line->size();
        const auto [parsedEnd, error] = std::from_chars(begin, end, value);
        if (error == std::errc{} && parsedEnd == end) {
            return value;
        }
        output_ << "Enter a whole number.\n";
    }
}

void CommandLineInterface::registerPassenger() {
    const auto id = readLine("Passenger ID: ");
    const auto name = readLine("Passenger name: ");
    const auto duration = readInteger("Screening duration in minutes: ");
    const auto priority = readInteger("Priority (lower number means more urgent): ");
    const auto timestamp = readInteger("Registration time in minutes: ");
    if (!id || !name || !duration || !priority || !timestamp) {
        output_ << "Registration cancelled because input closed.\n";
        return;
    }
    airport_.registerPassenger(Passenger(*id, *name, *duration, *priority), *timestamp);
    output_ << "Passenger registered.\n";
}

void CommandLineInterface::recordArrival() {
    const auto id = readLine("Passenger ID: ");
    const auto timestamp = readInteger("Arrival time in minutes: ");
    if (!id || !timestamp) {
        output_ << "Arrival cancelled because input closed.\n";
        return;
    }
    const ArrivalResult result = airport_.passengerArrives(*id, *timestamp);
    output_ << "Assigned to " << result.laneId
            << "; estimated wait: " << result.estimatedWaitMinutes << " minute(s).\n";
}

void CommandLineInterface::screenPassenger() {
    const auto laneId = readLine("Lane ID (Lane-1 or Lane-2): ");
    const auto timestamp = readInteger("Screening time in minutes: ");
    if (!laneId || !timestamp) {
        output_ << "Screening cancelled because input closed.\n";
        return;
    }
    if (airport_.screenNext(*laneId, *timestamp)) {
        output_ << "Next passenger screened.\n";
    } else {
        output_ << "That lane has no passengers waiting.\n";
    }
}

void CommandLineInterface::showQueues() const {
    for (const LaneQueueStatus& lane : airport_.queueManager().laneStatuses()) {
        output_ << lane.laneId << " ("
                << lane.officerName << "): "
                << lane.queuedPassengers << " passenger(s), "
                << lane.workloadMinutes << " workload minute(s)\n";
    }
}

void CommandLineInterface::findPassenger() {
    const auto id = readLine("Passenger ID: ");
    if (!id) {
        output_ << "Lookup cancelled because input closed.\n";
        return;
    }
    const Passenger* passenger = airport_.findPassenger(*id);
    if (passenger == nullptr) {
        output_ << "No passenger found with ID " << *id << ".\n";
        return;
    }
    output_ << passenger->id() << " - " << passenger->name()
            << ", priority " << passenger->priority() << '\n';
}

void CommandLineInterface::listPassengersByPriority() {
    const auto priority = readInteger("Priority to list: ");
    if (!priority) {
        output_ << "Priority lookup cancelled because input closed.\n";
        return;
    }
    const std::vector<const Passenger*> passengers =
        airport_.passengersByPriority(*priority);
    if (passengers.empty()) {
        output_ << "No passengers have priority " << *priority << ".\n";
        return;
    }
    for (const Passenger* passenger : passengers) {
        output_ << passenger->id() << " - " << passenger->name() << '\n';
    }
}

void CommandLineInterface::listPassengerIdsSorted() {
    const auto choice = readInteger("Sort with 1. Merge Sort or 2. Quick Sort: ");
    if (!choice) {
        output_ << "Listing cancelled because input closed.\n";
        return;
    }
    if (*choice != 1 && *choice != 2) {
        output_ << "Choose 1 for Merge Sort or 2 for Quick Sort.\n";
        return;
    }
    const PassengerIdSort algorithm =
        *choice == 1 ? PassengerIdSort::Merge : PassengerIdSort::Quick;
    for (const std::string& id : airport_.passengerIdsSortedBy(algorithm)) {
        output_ << id << '\n';
    }
}

} // namespace project31
