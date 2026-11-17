#include "simulator/CommandLineInterface.hpp"

#include "core/SecurityLane.hpp"
#include "core/SecurityOfficer.hpp"
#include "simulator/AirportOperations.hpp"

#include <algorithm>
#include <charconv>
#include <filesystem>
#include <iomanip>
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
                   "8. List passenger IDs using a sort algorithm\n"
                   "9. Run a built-in airport scenario\n"
                   "10. Load and compare a passenger CSV\n"
                   "11. Export the last scenario as CSV and JSON\n"
                   "12. Evaluate staffing against service targets\n";
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
            case 9:
                runBuiltInScenario();
                break;
            case 10:
                runUserCsvScenario();
                break;
            case 11:
                exportLastScenario();
                break;
            case 12:
                runStaffingEvaluation();
                break;
            default:
                output_ << "Choose a number from 1 to 12.\n";
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

void CommandLineInterface::runBuiltInScenario() {
    output_ << "1. Normal operation (sample CSV)\n"
               "2. Capacity/constraint shock (sample CSV and lane outage)\n"
               "3. Large-data stress (50,000 generated passengers)\n";
    const auto choice = readInteger("Scenario: ");
    if (!choice) {
        output_ << "Scenario cancelled because input closed.\n";
        return;
    }

    const std::filesystem::path dataDirectory("data");

    AirportScenario scenario;
    if (*choice == 1) {
        scenario = AirportScenarioRunner::loadCsvFile(
            dataDirectory / "normal.csv",
            "Normal operation",
            {2U, 2U, {}}
        );
    } else if (*choice == 2) {
        scenario = AirportScenarioRunner::loadCsvFile(
            dataDirectory / "capacity_shock.csv",
            "Capacity and staffing shock",
            {3U, 3U, {{0U, 3, 12}}}
        );
    } else if (*choice == 3) {
        scenario = AirportScenarioRunner::makeSyntheticScenario(
            "Large-data stress",
            50'000U,
            {6U, 6U, {}}
        );
    } else {
        output_ << "Choose scenario 1, 2, or 3.\n";
        return;
    }
    lastScenarioInput_ = scenario;
    lastScenario_ = AirportOperations::compareAirportStrategies(scenario);
    displayScenarioComparison(*lastScenario_);
}

void CommandLineInterface::runUserCsvScenario() {
    const auto path = readLine("Passenger CSV path: ");
    const auto laneCount = readInteger("Number of staffed lanes: ");
    const auto officerCount = readInteger("Number of available officers: ");
    if (!path || !laneCount || !officerCount) {
        output_ << "Scenario cancelled because input closed.\n";
        return;
    }
    if (*laneCount <= 0 || *officerCount <= 0 ||
        *laneCount > 127 || *officerCount > 127) {
        throw std::invalid_argument(
            "Comparison requires 1 to 127 baseline lanes and officers so the "
            "additional-staffing what-if stays within the 128-resource limit."
        );
    }
    AirportScenario scenario = AirportScenarioRunner::loadCsvFile(
        *path,
        "User-provided CSV",
        {
            static_cast<std::size_t>(*laneCount),
            static_cast<std::size_t>(*officerCount),
            {}
        }
    );
    lastScenarioInput_ = scenario;
    lastScenario_ = AirportOperations::compareAirportStrategies(scenario);
    displayScenarioComparison(*lastScenario_);
}

void CommandLineInterface::runStaffingEvaluation() {
    if (!lastScenarioInput_) {
        output_ << "Run or load a scenario before evaluating staffing.\n";
        return;
    }

    const auto minimumLanes = readInteger("Minimum lane/officer pairs (1-128): ");
    const auto maximumLanes = readInteger("Maximum lane/officer pairs (1-128): ");
    const auto maximumMeanWait = readInteger(
        "Maximum mean wait in minutes (-1 for no target): "
    );
    const auto maximumPassengerWait = readInteger(
        "Maximum individual wait in minutes (-1 for no target): "
    );
    const auto minimumThroughput = readInteger(
        "Minimum throughput per hour (-1 for no target): "
    );
    const auto laneCost = readInteger("Resource cost per lane (nonnegative): ");
    const auto officerCost = readInteger("Resource cost per officer (nonnegative): ");
    const auto maximumCost = readInteger("Maximum resource cost (-1 for no budget): ");
    if (!minimumLanes || !maximumLanes || !maximumMeanWait ||
        !maximumPassengerWait || !minimumThroughput || !laneCost ||
        !officerCost || !maximumCost) {
        output_ << "Staffing evaluation cancelled because input closed.\n";
        return;
    }
    if (*minimumLanes <= 0 || *maximumLanes < *minimumLanes ||
        *maximumLanes > 128 || *maximumMeanWait < -1 ||
        *maximumPassengerWait < -1 || *minimumThroughput < -1 ||
        *laneCost < 0 || *officerCost < 0 || *maximumCost < -1) {
        throw std::invalid_argument("Staffing evaluation inputs are outside their valid ranges.");
    }

    StaffingEvaluationOptions options;
    options.minimumLaneCount = static_cast<std::size_t>(*minimumLanes);
    options.maximumLaneCount = static_cast<std::size_t>(*maximumLanes);
    options.costPerLane = static_cast<std::uint64_t>(*laneCost);
    options.costPerOfficer = static_cast<std::uint64_t>(*officerCost);
    if (*maximumCost >= 0) {
        options.maximumCost = static_cast<std::uint64_t>(*maximumCost);
    }
    if (*maximumMeanWait >= 0) {
        options.maximumMeanWaitMinutes = static_cast<double>(*maximumMeanWait);
    }
    if (*maximumPassengerWait >= 0) {
        options.maximumPassengerWaitMinutes = *maximumPassengerWait;
    }
    if (*minimumThroughput >= 0) {
        options.minimumThroughputPerHour =
            static_cast<double>(*minimumThroughput);
    }

    const StaffingEvaluation evaluation =
        AirportOperations::recommendAirportStaffing(*lastScenarioInput_, options);
    for (const StaffingCandidateResult& candidate : evaluation.candidates) {
        output_ << candidate.laneCount << " lane/officer pair(s), cost "
                << candidate.resourceCost << ": mean wait "
                << std::fixed << std::setprecision(2)
                << candidate.run.meanWaitMinutes << " min, max wait "
                << candidate.run.maximumWaitMinutes << " min, throughput "
                << candidate.run.throughputPerHour << "/hour; "
                << (candidate.meetsTargets ? "meets targets" : "misses targets")
                << '\n';
    }
    if (!evaluation.recommendedCandidateIndex) {
        output_ << "No staffing configuration satisfies the specified targets "
                   "within the evaluated range and budget.\n";
        return;
    }
    const StaffingCandidateResult& recommendation =
        evaluation.candidates[*evaluation.recommendedCandidateIndex];
    output_ << "Lowest-cost feasible configuration: "
            << recommendation.laneCount << " lane/officer pair(s), cost "
            << recommendation.resourceCost << ".\n";
}

void CommandLineInterface::exportLastScenario() {
    if (!lastScenario_) {
        output_ << "Run a scenario before exporting results.\n";
        return;
    }
    const auto basePath = readLine("Export base path (without extension): ");
    if (!basePath) {
        output_ << "Export cancelled because input closed.\n";
        return;
    }
    const std::filesystem::path base(*basePath);
    AirportScenarioRunner::exportCsv(
        base.string() + ".csv",
        *lastScenario_
    );
    AirportScenarioRunner::exportJson(
        base.string() + ".json",
        *lastScenario_
    );
    output_ << "Exported CSV and JSON results to "
            << base.string() << ".csv and " << base.string() << ".json.\n";
}

void CommandLineInterface::displayScenarioComparison(
    const ScenarioComparison& comparison
) const {
    output_ << "\nScenario: " << comparison.scenarioName
            << "\nInput: " << comparison.inputSource
            << "\nPassenger count: " << comparison.shortestWorkload.passengerCount
            << (comparison.syntheticInput ? " (deterministic synthetic input)" : "")
            << '\n';
    const ScenarioRunResult* runs[]{
        &comparison.shortestWorkload,
        &comparison.roundRobin,
        &comparison.additionalLaneWhatIf
    };
    for (const ScenarioRunResult* run : runs) {
        output_ << "\nStrategy: " << toString(run->strategy)
                << " (" << run->staffedLaneCount << " staffed lanes, "
                << run->staffedOfficerCount << " officers)"
                << "\n  Completed: " << run->completedPassengerCount << '/'
                << run->passengerCount
                << "\n  Risk categories (high/elevated/standard): "
                << run->highRiskPassengerCount << '/'
                << run->elevatedRiskPassengerCount << '/'
                << run->standardRiskPassengerCount
                << "\n  Mean/max wait: " << std::fixed << std::setprecision(2)
                << run->meanWaitMinutes << '/' << run->maximumWaitMinutes
                << " minutes"
                << "\n  Throughput: " << run->throughputPerHour
                << " passengers/hour"
                << "\n  Runtime: " << run->elapsedMicroseconds << " microseconds"
                << "\n  Estimated result-storage bytes: " << run->estimatedMemoryBytes
                << " bytes\n";
        for (const LaneSimulationMetrics& lane : run->lanes) {
            output_ << "  " << lane.laneId << " / " << lane.officerId
                    << ": " << lane.passengerCount << " passenger(s), "
                    << lane.busyMinutes << '/' << lane.availableMinutes
                    << " available minutes busy ("
                    << std::setprecision(1) << lane.utilizationPercent << "%)\n";
        }
        const std::size_t displayedCount = std::min<std::size_t>(
            run->passengers.size(),
            run->passengers.size() <= 20U ? 20U : 5U
        );
        for (std::size_t index = 0; index < displayedCount; ++index) {
            const PassengerServiceRecord& passenger = run->passengers[index];
            output_ << "    " << passenger.passenger.id << " ("
                    << toString(passenger.riskCategory) << ") -> "
                    << passenger.laneId << '/' << passenger.officerId
                    << ", wait " << passenger.waitMinutes
                    << " minute(s), completed at "
                    << passenger.completionMinute << '\n';
        }
        if (displayedCount < run->passengers.size()) {
            output_ << "    ... " << run->passengers.size() - displayedCount
                    << " additional passenger assignment(s) in the exports.\n";
        }
    }
    output_ << "Recommended assignment strategy: "
            << comparison.recommendedStrategy << '\n'
            << "Staffing what-if: " << comparison.resourceRecommendation << '\n';
}

} // namespace project31
