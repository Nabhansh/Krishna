#include "simulator/AirportScenario.hpp"

#include "core/Passenger.hpp"
#include "core/SecurityLane.hpp"
#include "core/SecurityOfficer.hpp"

#include <algorithm>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <iterator>
#include <optional>
#include <queue>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <unordered_set>
#include <utility>

namespace project31 {
namespace {

constexpr std::size_t kMaximumStaffedResources = 128U;

bool isBlank(std::string_view value) {
    return value.find_first_not_of(" \t\n\r\f\v") == std::string_view::npos;
}

std::vector<std::string> parseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::size_t position = 0;
    while (true) {
        std::string field;
        if (position < line.size() && line[position] == '"') {
            ++position;
            bool closed = false;
            while (position < line.size()) {
                if (line[position] != '"') {
                    field.push_back(line[position++]);
                } else if (position + 1U < line.size() &&
                           line[position + 1U] == '"') {
                    field.push_back('"');
                    position += 2U;
                } else {
                    ++position;
                    closed = true;
                    break;
                }
            }
            if (!closed || (position < line.size() && line[position] != ',')) {
                throw std::invalid_argument("Malformed quoted CSV field.");
            }
        } else {
            while (position < line.size() && line[position] != ',') {
                if (line[position] == '"') {
                    throw std::invalid_argument("Unexpected quote in CSV field.");
                }
                field.push_back(line[position++]);
            }
        }
        fields.push_back(std::move(field));
        if (position == line.size()) {
            break;
        }
        ++position;
        if (position == line.size()) {
            fields.emplace_back();
            break;
        }
    }
    return fields;
}

int parseInteger(const std::string& value, std::string_view fieldName) {
    int result{};
    const char* const begin = value.data();
    const char* const end = begin + value.size();
    const auto [parsedEnd, error] = std::from_chars(begin, end, result);
    if (error != std::errc{} || parsedEnd != end) {
        throw std::invalid_argument(
            "CSV field '" + std::string(fieldName) + "' must be a whole number."
        );
    }
    return result;
}

std::vector<std::size_t> parseEligibleLanes(const std::string& value) {
    std::vector<std::size_t> lanes;
    if (value.empty()) {
        return lanes;
    }

    std::size_t tokenStart = 0U;
    while (tokenStart < value.size()) {
        const std::size_t separator = value.find('|', tokenStart);
        const std::size_t tokenEnd = separator == std::string::npos
            ? value.size()
            : separator;
        const std::string_view token(value.data() + tokenStart, tokenEnd - tokenStart);
        constexpr std::string_view prefix = "Lane-";
        if (!token.starts_with(prefix)) {
            throw std::invalid_argument(
                "eligible_lanes must contain IDs such as Lane-1 separated by '|'."
            );
        }
        std::size_t oneBasedIndex{};
        const char* const begin = token.data() + prefix.size();
        const char* const end = token.data() + token.size();
        const auto [parsedEnd, error] = std::from_chars(begin, end, oneBasedIndex);
        if (error != std::errc{} || parsedEnd != end || oneBasedIndex == 0U) {
            throw std::invalid_argument(
                "eligible_lanes must contain positive lane IDs such as Lane-1."
            );
        }
        lanes.push_back(oneBasedIndex - 1U);
        if (separator == std::string::npos) {
            break;
        }
        tokenStart = separator + 1U;
        if (tokenStart == value.size()) {
            throw std::invalid_argument("eligible_lanes contains an empty lane ID.");
        }
    }
    return lanes;
}

void validateScenario(const AirportScenario& scenario) {
    if (isBlank(scenario.name)) {
        throw std::invalid_argument("Scenario name must not be empty.");
    }
    if (scenario.configuration.laneCount == 0U) {
        throw std::invalid_argument("A scenario must have at least one staffed lane.");
    }
    if (scenario.configuration.laneCount > kMaximumStaffedResources ||
        scenario.configuration.officerCount > kMaximumStaffedResources) {
        throw std::length_error("Scenario staffing is limited to 128 lanes and officers.");
    }
    if (scenario.configuration.officerCount != scenario.configuration.laneCount) {
        throw std::invalid_argument(
            "This simulator assigns exactly one available officer to each active lane."
        );
    }
    if (scenario.configuration.priorityAgingIntervalMinutes <= 0) {
        throw std::invalid_argument("Priority aging interval must be positive.");
    }

    for (std::size_t left = 0; left < scenario.configuration.outages.size(); ++left) {
        const LaneOutage& outage = scenario.configuration.outages[left];
        if (outage.laneIndex >= scenario.configuration.laneCount) {
            throw std::out_of_range("Lane outage references an unknown lane.");
        }
        if (outage.startMinute < 0 || outage.endMinute <= outage.startMinute) {
            throw std::invalid_argument(
                "Lane outage must have a nonnegative start and a later end."
            );
        }
        for (std::size_t right = left + 1U;
             right < scenario.configuration.outages.size();
             ++right) {
            const LaneOutage& other = scenario.configuration.outages[right];
            if (outage.laneIndex == other.laneIndex &&
                outage.startMinute < other.endMinute &&
                other.startMinute < outage.endMinute) {
                throw std::invalid_argument(
                    "Outage intervals for one lane must not overlap."
                );
            }
        }
    }

    for (std::size_t left = 0;
         left < scenario.configuration.officerOutages.size();
         ++left) {
        const OfficerOutage& outage = scenario.configuration.officerOutages[left];
        if (outage.officerIndex >= scenario.configuration.officerCount) {
            throw std::out_of_range("Officer outage references an unknown officer.");
        }
        if (outage.startMinute < 0 || outage.endMinute <= outage.startMinute) {
            throw std::invalid_argument(
                "Officer outage must have a nonnegative start and a later end."
            );
        }
        for (std::size_t right = left + 1U;
             right < scenario.configuration.officerOutages.size();
             ++right) {
            const OfficerOutage& other =
                scenario.configuration.officerOutages[right];
            if (outage.officerIndex == other.officerIndex &&
                outage.startMinute < other.endMinute &&
                other.startMinute < outage.endMinute) {
                throw std::invalid_argument(
                    "Outage intervals for one officer must not overlap."
                );
            }
        }
    }

    std::unordered_set<std::string> passengerIds;
    passengerIds.reserve(scenario.passengers.size());
    for (const ScenarioPassenger& record : scenario.passengers) {
        const Passenger passenger(
            record.id,
            record.name,
            record.screeningDurationMinutes,
            record.priority
        );
        const std::vector<std::string> errors = passenger.validationErrors();
        if (!errors.empty()) {
            throw std::invalid_argument(errors.front());
        }
        if (record.arrivalMinute < 0) {
            throw std::invalid_argument("Passenger arrival time must not be negative.");
        }
        (void)riskCategoryForPriority(record.priority);
        std::unordered_set<std::size_t> eligibleLanes;
        for (const std::size_t laneIndex : record.eligibleLaneIndices) {
            if (laneIndex >= scenario.configuration.laneCount) {
                throw std::out_of_range(
                    "Passenger lane eligibility references an unknown lane."
                );
            }
            if (!eligibleLanes.insert(laneIndex).second) {
                throw std::invalid_argument(
                    "Passenger lane eligibility must not contain duplicates."
                );
            }
        }
        if (!passengerIds.insert(record.id).second) {
            throw std::invalid_argument("Passenger IDs must be unique: " + record.id);
        }
    }
}

std::int64_t checkedAdd(std::int64_t left, std::int64_t right, const char* message) {
    if (right > 0 && left > std::numeric_limits<std::int64_t>::max() - right) {
        throw std::overflow_error(message);
    }
    return left + right;
}

std::int64_t feasibleServiceStart(
    std::size_t laneIndex,
    std::int64_t earliestStart,
    int duration,
    const ScenarioConfiguration& configuration
) {
    std::int64_t candidate = earliestStart;
    while (true) {
        const std::int64_t candidateEnd = checkedAdd(
            candidate,
            duration,
            "Scenario completion time exceeds the supported range."
        );
        std::int64_t nextCandidate = candidate;
        for (const LaneOutage& outage : configuration.outages) {
            if (outage.laneIndex != laneIndex) {
                continue;
            }
            if (candidate < outage.endMinute && outage.startMinute < candidateEnd) {
                nextCandidate = std::max<std::int64_t>(
                    nextCandidate,
                    outage.endMinute
                );
            }
        }
        for (const OfficerOutage& outage : configuration.officerOutages) {
            if (outage.officerIndex != laneIndex) {
                continue;
            }
            if (candidate < outage.endMinute && outage.startMinute < candidateEnd) {
                nextCandidate = std::max<std::int64_t>(
                    nextCandidate,
                    outage.endMinute
                );
            }
        }
        if (nextCandidate == candidate) {
            return candidate;
        }
        candidate = nextCandidate;
    }
}

bool laneEligibleFor(const ScenarioPassenger& passenger, std::size_t laneIndex) {
    return passenger.eligibleLaneIndices.empty() ||
        std::find(
            passenger.eligibleLaneIndices.begin(),
            passenger.eligibleLaneIndices.end(),
            laneIndex
        ) != passenger.eligibleLaneIndices.end();
}

enum class ScenarioEventType {
    ScreeningCompleted,
    LaneOutageEnded,
    OfficerOutageEnded,
    LaneOutageStarted,
    OfficerOutageStarted,
    PassengerAged,
    PassengerArrived
};

struct ScenarioEvent {
    std::int64_t time;
    ScenarioEventType type;
    std::size_t sequence;
    std::size_t reference;
};

int eventPrecedence(ScenarioEventType type) {
    switch (type) {
    case ScenarioEventType::ScreeningCompleted:
        return 0;
    case ScenarioEventType::LaneOutageEnded:
    case ScenarioEventType::OfficerOutageEnded:
        return 1;
    case ScenarioEventType::LaneOutageStarted:
    case ScenarioEventType::OfficerOutageStarted:
        return 2;
    case ScenarioEventType::PassengerAged:
        return 3;
    case ScenarioEventType::PassengerArrived:
        return 4;
    }
    return 4;
}

struct LaterScenarioEvent {
    bool operator()(const ScenarioEvent& left, const ScenarioEvent& right) const {
        return std::tuple(
                   left.time,
                   eventPrecedence(left.type),
                   left.sequence
               ) >
            std::tuple(
                   right.time,
                   eventPrecedence(right.type),
                   right.sequence
               );
    }
};

std::int64_t unavailableMinutes(
    std::size_t laneIndex,
    std::int64_t start,
    std::int64_t end,
    const ScenarioConfiguration& configuration
) {
    std::vector<std::pair<std::int64_t, std::int64_t>> intervals;
    for (const LaneOutage& outage : configuration.outages) {
        if (outage.laneIndex == laneIndex) {
            intervals.emplace_back(
                std::max<std::int64_t>(start, outage.startMinute),
                std::min<std::int64_t>(end, outage.endMinute)
            );
        }
    }
    for (const OfficerOutage& outage : configuration.officerOutages) {
        if (outage.officerIndex == laneIndex) {
            intervals.emplace_back(
                std::max<std::int64_t>(start, outage.startMinute),
                std::min<std::int64_t>(end, outage.endMinute)
            );
        }
    }
    std::sort(intervals.begin(), intervals.end());
    std::int64_t unavailable = 0;
    std::int64_t mergedStart = 0;
    std::int64_t mergedEnd = 0;
    bool hasInterval = false;
    for (const auto& [intervalStart, intervalEnd] : intervals) {
        if (intervalEnd <= intervalStart) {
            continue;
        }
        if (!hasInterval) {
            mergedStart = intervalStart;
            mergedEnd = intervalEnd;
            hasInterval = true;
        } else if (intervalStart <= mergedEnd) {
            mergedEnd = std::max(mergedEnd, intervalEnd);
        } else {
            unavailable = checkedAdd(
                unavailable,
                mergedEnd - mergedStart,
                "Total unavailable time exceeds the supported range."
            );
            mergedStart = intervalStart;
            mergedEnd = intervalEnd;
        }
    }
    if (hasInterval) {
        unavailable = checkedAdd(
            unavailable,
            mergedEnd - mergedStart,
            "Total unavailable time exceeds the supported range."
        );
    }
    return unavailable;
}

std::string createRunId() {
    static std::atomic<std::uint64_t> sequence{0U};
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    std::ostringstream result;
    result << milliseconds << '-' << sequence.fetch_add(1U);
    return result.str();
}

void checkedSizeAdd(std::size_t& total, std::size_t amount) {
    if (total > std::numeric_limits<std::size_t>::max() - amount) {
        throw std::overflow_error("Scenario memory estimate exceeds the supported range.");
    }
    total += amount;
}

std::size_t checkedSizeMultiply(std::size_t left, std::size_t right) {
    if (right != 0U && left > std::numeric_limits<std::size_t>::max() / right) {
        throw std::overflow_error("Scenario memory estimate exceeds the supported range.");
    }
    return left * right;
}

void addStringStorageEstimate(std::size_t& total, const std::string& value) {
    checkedSizeAdd(total, value.capacity());
    checkedSizeAdd(total, 1U);
}

std::size_t estimateMemoryBytes(const ScenarioRunResult& result) {
    std::size_t total = sizeof(ScenarioRunResult);
    addStringStorageEstimate(total, result.scenarioName);
    addStringStorageEstimate(total, result.inputSource);
    addStringStorageEstimate(total, result.runId);
    checkedSizeAdd(
        total,
        checkedSizeMultiply(result.lanes.capacity(), sizeof(LaneSimulationMetrics))
    );
    checkedSizeAdd(
        total,
        checkedSizeMultiply(result.passengers.capacity(), sizeof(PassengerServiceRecord))
    );
    for (const LaneSimulationMetrics& lane : result.lanes) {
        addStringStorageEstimate(total, lane.laneId);
        addStringStorageEstimate(total, lane.officerId);
    }
    for (const PassengerServiceRecord& passenger : result.passengers) {
        addStringStorageEstimate(total, passenger.passenger.id);
        addStringStorageEstimate(total, passenger.passenger.name);
        addStringStorageEstimate(total, passenger.laneId);
        addStringStorageEstimate(total, passenger.officerId);
    }
    return total;
}

ScenarioRunResult runScenario(
    const AirportScenario& scenario,
    AssignmentStrategy strategy
) {
    validateScenario(scenario);
    const auto started = std::chrono::steady_clock::now();
    ScenarioRunResult result{
        scenario.name,
        scenario.inputSource,
        createRunId(),
        strategy,
        scenario.passengers.size(),
        0U,
        0U,
        0U,
        0U,
        0,
        0.0,
        0,
        0,
        0.0,
        scenario.configuration.laneCount,
        scenario.configuration.officerCount,
        0U,
        0U,
        {},
        {}
    };
    result.lanes.reserve(scenario.configuration.laneCount);
    std::vector<SecurityLane> staffedLanes;
    staffedLanes.reserve(scenario.configuration.laneCount);
    for (std::size_t lane = 0; lane < scenario.configuration.laneCount; ++lane) {
        const std::string suffix = std::to_string(lane + 1U);
        SecurityOfficer officer(
            "Officer-" + suffix,
            "Security Officer " + suffix
        );
        staffedLanes.emplace_back("Lane-" + suffix, officer);
        result.lanes.push_back({
            staffedLanes.back().id(),
            staffedLanes.back().officer().id(),
            0U,
            0,
            0,
            0.0
        });
    }

    using WaitingKey = std::tuple<int, std::int64_t, std::size_t, std::size_t>;
    const std::size_t passengerCount = scenario.passengers.size();
    std::vector<Passenger> passengers;
    std::vector<std::vector<std::size_t>> eligibleLanes(passengerCount);
    passengers.reserve(passengerCount);
    for (std::size_t index = 0; index < passengerCount; ++index) {
        const ScenarioPassenger& record = scenario.passengers[index];
        passengers.emplace_back(
            record.id,
            record.name,
            record.screeningDurationMinutes,
            record.priority
        );
        if (record.eligibleLaneIndices.empty()) {
            eligibleLanes[index].reserve(scenario.configuration.laneCount);
            for (std::size_t lane = 0; lane < scenario.configuration.laneCount; ++lane) {
                eligibleLanes[index].push_back(lane);
            }
        } else {
            eligibleLanes[index] = record.eligibleLaneIndices;
            std::sort(eligibleLanes[index].begin(), eligibleLanes[index].end());
        }
    }

    std::vector<std::set<WaitingKey>> waitingByLane(
        scenario.configuration.laneCount
    );
    std::vector<int> priorityRanks(passengerCount, 0);
    std::vector<bool> passengerArrived(passengerCount, false);
    std::vector<bool> screeningStarted(passengerCount, false);
    std::vector<bool> passengerCompleted(passengerCount, false);
    std::vector<std::int64_t> serviceStartTimes(passengerCount, 0);
    std::vector<std::int64_t> completionTimes(passengerCount, 0);
    std::vector<std::int64_t> waitTimes(passengerCount, 0);
    std::vector<std::size_t> assignedLanes(passengerCount, 0U);
    std::vector<std::optional<std::size_t>> activePassengerByLane(
        scenario.configuration.laneCount
    );
    std::vector<bool> laneClosed(scenario.configuration.laneCount, false);
    std::vector<bool> officerUnavailable(scenario.configuration.officerCount, false);
    std::vector<std::int64_t> assignedWorkMinutes(scenario.configuration.laneCount, 0);
    std::size_t nextEventSequence = 0U;
    std::priority_queue<
        ScenarioEvent,
        std::vector<ScenarioEvent>,
        LaterScenarioEvent
    > events;
    const auto scheduleEvent = [&events, &nextEventSequence](
                                   std::int64_t time,
                                   ScenarioEventType type,
                                   std::size_t reference
                               ) {
        events.push({time, type, nextEventSequence++, reference});
    };

    for (std::size_t index = 0; index < scenario.configuration.outages.size(); ++index) {
        const LaneOutage& outage = scenario.configuration.outages[index];
        scheduleEvent(
            outage.startMinute,
            ScenarioEventType::LaneOutageStarted,
            outage.laneIndex
        );
        scheduleEvent(
            outage.endMinute,
            ScenarioEventType::LaneOutageEnded,
            outage.laneIndex
        );
    }
    for (std::size_t index = 0;
         index < scenario.configuration.officerOutages.size();
         ++index) {
        const OfficerOutage& outage = scenario.configuration.officerOutages[index];
        scheduleEvent(
            outage.startMinute,
            ScenarioEventType::OfficerOutageStarted,
            outage.officerIndex
        );
        scheduleEvent(
            outage.endMinute,
            ScenarioEventType::OfficerOutageEnded,
            outage.officerIndex
        );
    }
    for (std::size_t index = 0; index < passengerCount; ++index) {
        scheduleEvent(
            scenario.passengers[index].arrivalMinute,
            ScenarioEventType::PassengerArrived,
            index
        );
    }

    std::size_t roundRobinLane = 0U;
    while (!events.empty() && result.completedPassengerCount < passengerCount) {
        const std::int64_t now = events.top().time;
        while (!events.empty() && events.top().time == now) {
            const ScenarioEvent event = events.top();
            events.pop();
            switch (event.type) {
            case ScenarioEventType::ScreeningCompleted: {
                const std::size_t laneIndex = event.reference;
                if (!activePassengerByLane[laneIndex]) {
                    throw std::logic_error(
                        "Screening completion has no active passenger."
                    );
                }
                const std::size_t passengerIndex = *activePassengerByLane[laneIndex];
                if (completionTimes[passengerIndex] != now ||
                    passengerCompleted[passengerIndex]) {
                    throw std::logic_error(
                        "Passenger screening completion was duplicated or mistimed."
                    );
                }
                const auto& laneQueue = staffedLanes[laneIndex].queue();
                if (laneQueue.size() != 1U ||
                    laneQueue.front().id() != passengers[passengerIndex].id()) {
                    throw std::logic_error(
                        "Security lane queue does not contain exactly its active passenger."
                    );
                }
                std::optional<Passenger> screened =
                    staffedLanes[laneIndex].screenNext();
                if (!screened || screened->id() != passengers[passengerIndex].id()) {
                    throw std::logic_error(
                        "Security lane queue does not match the active screening."
                    );
                }
                screened->markScreened();
                passengers[passengerIndex].markScreened();
                passengerCompleted[passengerIndex] = true;
                activePassengerByLane[laneIndex].reset();
                LaneSimulationMetrics& lane = result.lanes[laneIndex];
                ++lane.passengerCount;
                lane.busyMinutes = checkedAdd(
                    lane.busyMinutes,
                    scenario.passengers[passengerIndex].screeningDurationMinutes,
                    "Lane busy time exceeds the supported range."
                );
                ++result.completedPassengerCount;
                result.makespanMinutes = std::max(result.makespanMinutes, now);
                break;
            }
            case ScenarioEventType::LaneOutageEnded:
                laneClosed[event.reference] = false;
                break;
            case ScenarioEventType::OfficerOutageEnded:
                officerUnavailable[event.reference] = false;
                break;
            case ScenarioEventType::LaneOutageStarted:
                if (activePassengerByLane[event.reference]) {
                    throw std::logic_error(
                        "A lane outage would interrupt an active screening."
                    );
                }
                laneClosed[event.reference] = true;
                break;
            case ScenarioEventType::OfficerOutageStarted:
                if (activePassengerByLane[event.reference]) {
                    throw std::logic_error(
                        "An officer outage would interrupt an active screening."
                    );
                }
                officerUnavailable[event.reference] = true;
                break;
            case ScenarioEventType::PassengerAged: {
                const std::size_t passengerIndex = event.reference;
                if (!passengerArrived[passengerIndex] ||
                    screeningStarted[passengerIndex] ||
                    priorityRanks[passengerIndex] == 0) {
                    break;
                }
                const ScenarioPassenger& record = scenario.passengers[passengerIndex];
                const WaitingKey oldKey{
                    priorityRanks[passengerIndex],
                    record.arrivalMinute,
                    passengerIndex,
                    passengerIndex
                };
                for (const std::size_t laneIndex : eligibleLanes[passengerIndex]) {
                    waitingByLane[laneIndex].erase(oldKey);
                }
                --priorityRanks[passengerIndex];
                const WaitingKey newKey{
                    priorityRanks[passengerIndex],
                    record.arrivalMinute,
                    passengerIndex,
                    passengerIndex
                };
                for (const std::size_t laneIndex : eligibleLanes[passengerIndex]) {
                    waitingByLane[laneIndex].insert(newKey);
                }
                if (priorityRanks[passengerIndex] > 0) {
                    scheduleEvent(
                        checkedAdd(
                            now,
                            scenario.configuration.priorityAgingIntervalMinutes,
                            "Passenger priority aging time exceeds the supported range."
                        ),
                        ScenarioEventType::PassengerAged,
                        passengerIndex
                    );
                }
                break;
            }
            case ScenarioEventType::PassengerArrived: {
                const std::size_t passengerIndex = event.reference;
                if (passengerArrived[passengerIndex]) {
                    throw std::logic_error("Passenger arrival event was duplicated.");
                }
                Passenger& passenger = passengers[passengerIndex];
                passenger.markQueued();
                passengerArrived[passengerIndex] = true;
                const ScenarioPassenger& record = scenario.passengers[passengerIndex];
                const PassengerRiskCategory category =
                    riskCategoryForPriority(record.priority);
                switch (category) {
                case PassengerRiskCategory::High:
                    ++result.highRiskPassengerCount;
                    break;
                case PassengerRiskCategory::Elevated:
                    ++result.elevatedRiskPassengerCount;
                    break;
                case PassengerRiskCategory::Standard:
                    ++result.standardRiskPassengerCount;
                    break;
                }
                priorityRanks[passengerIndex] =
                    effectivePriorityRank(
                        category,
                        0,
                        scenario.configuration.priorityAgingIntervalMinutes
                    );
                const WaitingKey key{
                    priorityRanks[passengerIndex],
                    record.arrivalMinute,
                    passengerIndex,
                    passengerIndex
                };
                for (const std::size_t laneIndex : eligibleLanes[passengerIndex]) {
                    waitingByLane[laneIndex].insert(key);
                }
                if (priorityRanks[passengerIndex] > 0) {
                    scheduleEvent(
                        checkedAdd(
                            now,
                            scenario.configuration.priorityAgingIntervalMinutes,
                            "Passenger priority aging time exceeds the supported range."
                        ),
                        ScenarioEventType::PassengerAged,
                        passengerIndex
                    );
                }
                break;
            }
            }
        }

        while (true) {
            std::optional<WaitingKey> selectedKey;
            for (std::size_t laneIndex = 0;
                 laneIndex < scenario.configuration.laneCount;
                 ++laneIndex) {
                if (laneClosed[laneIndex] ||
                    officerUnavailable[laneIndex] ||
                    activePassengerByLane[laneIndex]) {
                    continue;
                }
                for (auto entry = waitingByLane[laneIndex].begin();
                     entry != waitingByLane[laneIndex].end();
                     ++entry) {
                    const std::size_t passengerIndex = std::get<3>(*entry);
                    if (feasibleServiceStart(
                            laneIndex,
                            now,
                            scenario.passengers[passengerIndex].screeningDurationMinutes,
                            scenario.configuration
                        ) == now) {
                        if (!selectedKey || *entry < *selectedKey) {
                            selectedKey = *entry;
                        }
                        break;
                    }
                }
            }
            if (!selectedKey) {
                break;
            }

            const std::size_t passengerIndex = std::get<3>(*selectedKey);
            if (!passengerArrived[passengerIndex] ||
                screeningStarted[passengerIndex] ||
                passengerCompleted[passengerIndex]) {
                throw std::logic_error(
                    "Dispatcher selected a passenger outside the waiting state."
                );
            }
            const ScenarioPassenger& record = scenario.passengers[passengerIndex];
            std::vector<std::size_t> candidateLanes;
            for (std::size_t laneIndex = 0;
                 laneIndex < scenario.configuration.laneCount;
                 ++laneIndex) {
                if (!laneClosed[laneIndex] &&
                    !officerUnavailable[laneIndex] &&
                    !activePassengerByLane[laneIndex] &&
                    laneEligibleFor(record, laneIndex) &&
                    feasibleServiceStart(
                        laneIndex,
                        now,
                        record.screeningDurationMinutes,
                        scenario.configuration
                    ) == now) {
                    candidateLanes.push_back(laneIndex);
                }
            }
            if (candidateLanes.empty()) {
                throw std::logic_error(
                    "Selected passenger has no available eligible screening lane."
                );
            }

            std::size_t selectedLane = candidateLanes.front();
            if (strategy == AssignmentStrategy::RoundRobin) {
                for (std::size_t offset = 0;
                     offset < scenario.configuration.laneCount;
                     ++offset) {
                    const std::size_t candidate =
                        (roundRobinLane + offset) % scenario.configuration.laneCount;
                    if (std::find(
                            candidateLanes.begin(),
                            candidateLanes.end(),
                            candidate
                        ) != candidateLanes.end()) {
                        selectedLane = candidate;
                        break;
                    }
                }
                roundRobinLane =
                    (selectedLane + 1U) % scenario.configuration.laneCount;
            } else {
                for (const std::size_t candidate : candidateLanes) {
                    if (assignedWorkMinutes[candidate] <
                        assignedWorkMinutes[selectedLane]) {
                        selectedLane = candidate;
                    }
                }
            }

            for (const std::size_t laneIndex : eligibleLanes[passengerIndex]) {
                waitingByLane[laneIndex].erase(*selectedKey);
            }
            screeningStarted[passengerIndex] = true;
            assignedLanes[passengerIndex] = selectedLane;
            serviceStartTimes[passengerIndex] = now;
            waitTimes[passengerIndex] =
                now - record.arrivalMinute;
            result.totalWaitMinutes = checkedAdd(
                result.totalWaitMinutes,
                waitTimes[passengerIndex],
                "Total passenger wait exceeds the supported range."
            );
            result.maximumWaitMinutes = std::max(
                result.maximumWaitMinutes,
                waitTimes[passengerIndex]
            );
            assignedWorkMinutes[selectedLane] = checkedAdd(
                assignedWorkMinutes[selectedLane],
                record.screeningDurationMinutes,
                "Lane assigned workload exceeds the supported range."
            );
            if (activePassengerByLane[selectedLane] ||
                !staffedLanes[selectedLane].queue().empty()) {
                throw std::logic_error(
                    "Dispatcher attempted to occupy an already active lane."
                );
            }
            staffedLanes[selectedLane].enqueue(passengers[passengerIndex]);
            activePassengerByLane[selectedLane] = passengerIndex;
            completionTimes[passengerIndex] = checkedAdd(
                now,
                record.screeningDurationMinutes,
                "Scenario completion time exceeds the supported range."
            );
            scheduleEvent(
                completionTimes[passengerIndex],
                ScenarioEventType::ScreeningCompleted,
                selectedLane
            );
        }
    }

    if (result.completedPassengerCount != passengerCount) {
        throw std::logic_error(
            "Scenario ended before every accepted passenger completed screening."
        );
    }
    result.passengers.reserve(passengerCount);
    for (std::size_t index = 0; index < passengerCount; ++index) {
        if (!screeningStarted[index] || !passengerCompleted[index] ||
            passengers[index].status() != PassengerStatus::Screened) {
            throw std::logic_error(
                "Accepted passenger was not screened exactly once."
            );
        }
        const std::size_t laneIndex = assignedLanes[index];
        const LaneSimulationMetrics& lane = result.lanes[laneIndex];
        result.passengers.push_back({
            scenario.passengers[index],
            riskCategoryForPriority(scenario.passengers[index].priority),
            lane.laneId,
            lane.officerId,
            serviceStartTimes[index],
            completionTimes[index],
            waitTimes[index]
        });
    }

    const std::int64_t simulationStartMinute = scenario.passengers.empty()
        ? 0
        : scenario.passengers.front().arrivalMinute;
    const std::int64_t simulationFinishMinute = result.makespanMinutes;
    result.makespanMinutes = simulationFinishMinute - simulationStartMinute;
    if (result.passengerCount > 0U) {
        result.meanWaitMinutes =
            static_cast<double>(result.totalWaitMinutes) /
            static_cast<double>(result.passengerCount);
        result.throughputPerHour =
            static_cast<double>(result.completedPassengerCount) * 60.0 /
            static_cast<double>(result.makespanMinutes);
    }
    for (std::size_t laneIndex = 0; laneIndex < result.lanes.size(); ++laneIndex) {
        LaneSimulationMetrics& lane = result.lanes[laneIndex];
        lane.availableMinutes = result.makespanMinutes;
        lane.availableMinutes -= unavailableMinutes(
            laneIndex,
            simulationStartMinute,
            simulationFinishMinute,
            scenario.configuration
        );
        if (lane.availableMinutes > 0) {
            lane.utilizationPercent = 100.0 *
                static_cast<double>(lane.busyMinutes) /
                static_cast<double>(lane.availableMinutes);
        }
    }
    const auto finished = std::chrono::steady_clock::now();
    result.elapsedMicroseconds =
        static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::microseconds>(finished - started).count()
        );
    result.estimatedMemoryBytes = estimateMemoryBytes(result);
    return result;
}

bool resultsCoverScenario(
    const ScenarioRunResult& result,
    const AirportScenario& scenario
) {
    if (result.passengerCount != scenario.passengers.size() ||
        result.completedPassengerCount != scenario.passengers.size() ||
        result.passengers.size() != scenario.passengers.size() ||
        result.lanes.size() != scenario.configuration.laneCount) {
        return false;
    }
    std::vector<std::vector<std::pair<std::int64_t, std::int64_t>>> laneIntervals(
        result.lanes.size()
    );
    std::vector<std::int64_t> laneBusyMinutes(result.lanes.size(), 0);
    std::vector<std::size_t> lanePassengerCounts(result.lanes.size(), 0U);
    std::int64_t totalWaitMinutes = 0;
    std::int64_t maximumWaitMinutes = 0;
    std::int64_t finalCompletionMinute = 0;
    std::size_t highRiskCount = 0U;
    std::size_t elevatedRiskCount = 0U;
    std::size_t standardRiskCount = 0U;
    for (std::size_t index = 0; index < result.passengers.size(); ++index) {
        const PassengerServiceRecord& service = result.passengers[index];
        const ScenarioPassenger& expected = scenario.passengers[index];
        if (service.passenger.id != expected.id ||
            service.passenger.name != expected.name ||
            service.passenger.arrivalMinute != expected.arrivalMinute ||
            service.passenger.screeningDurationMinutes !=
                expected.screeningDurationMinutes ||
            service.passenger.priority != expected.priority ||
            service.serviceStartMinute < expected.arrivalMinute ||
            service.completionMinute - service.serviceStartMinute !=
                expected.screeningDurationMinutes ||
            service.waitMinutes !=
                service.serviceStartMinute - expected.arrivalMinute ||
            service.riskCategory != riskCategoryForPriority(expected.priority)) {
            return false;
        }
        switch (service.riskCategory) {
        case PassengerRiskCategory::High:
            ++highRiskCount;
            break;
        case PassengerRiskCategory::Elevated:
            ++elevatedRiskCount;
            break;
        case PassengerRiskCategory::Standard:
            ++standardRiskCount;
            break;
        }
        const auto lane = std::find_if(
            result.lanes.begin(),
            result.lanes.end(),
            [&service](const LaneSimulationMetrics& candidate) {
                return candidate.laneId == service.laneId &&
                    candidate.officerId == service.officerId;
            });
        if (lane == result.lanes.end()) {
            return false;
        }
        const std::size_t laneIndex = static_cast<std::size_t>(
            std::distance(result.lanes.begin(), lane)
        );
        if (!laneEligibleFor(expected, laneIndex)) {
            return false;
        }
        laneIntervals[laneIndex].emplace_back(
            service.serviceStartMinute,
            service.completionMinute
        );
        ++lanePassengerCounts[laneIndex];
        laneBusyMinutes[laneIndex] = checkedAdd(
            laneBusyMinutes[laneIndex],
            expected.screeningDurationMinutes,
            "Lane busy time exceeds the supported range."
        );
        totalWaitMinutes = checkedAdd(
            totalWaitMinutes,
            service.waitMinutes,
            "Total passenger wait exceeds the supported range."
        );
        maximumWaitMinutes = std::max(maximumWaitMinutes, service.waitMinutes);
        finalCompletionMinute = std::max(
            finalCompletionMinute,
            service.completionMinute
        );
        for (const LaneOutage& outage : scenario.configuration.outages) {
            if (outage.laneIndex == laneIndex &&
                service.serviceStartMinute < outage.endMinute &&
                outage.startMinute < service.completionMinute) {
                return false;
            }
        }
        for (const OfficerOutage& outage : scenario.configuration.officerOutages) {
            if (outage.officerIndex == laneIndex &&
                service.serviceStartMinute < outage.endMinute &&
                outage.startMinute < service.completionMinute) {
                return false;
            }
        }
    }
    const std::int64_t simulationStartMinute = scenario.passengers.empty()
        ? 0
        : scenario.passengers.front().arrivalMinute;
    const std::int64_t expectedMakespan =
        finalCompletionMinute - simulationStartMinute;
    if (result.totalWaitMinutes != totalWaitMinutes ||
        result.maximumWaitMinutes != maximumWaitMinutes ||
        result.makespanMinutes != expectedMakespan ||
        result.highRiskPassengerCount != highRiskCount ||
        result.elevatedRiskPassengerCount != elevatedRiskCount ||
        result.standardRiskPassengerCount != standardRiskCount) {
        return false;
    }
    const double expectedMeanWait = result.passengerCount == 0U
        ? 0.0
        : static_cast<double>(totalWaitMinutes) /
            static_cast<double>(result.passengerCount);
    const double expectedThroughput = expectedMakespan <= 0
        ? 0.0
        : static_cast<double>(result.completedPassengerCount) * 60.0 /
            static_cast<double>(expectedMakespan);
    if (std::abs(result.meanWaitMinutes - expectedMeanWait) > 1e-9 ||
        std::abs(result.throughputPerHour - expectedThroughput) > 1e-9) {
        return false;
    }
    for (std::size_t laneIndex = 0; laneIndex < result.lanes.size(); ++laneIndex) {
        const LaneSimulationMetrics& lane = result.lanes[laneIndex];
        std::sort(laneIntervals[laneIndex].begin(), laneIntervals[laneIndex].end());
        std::int64_t previousCompletion = 0;
        for (const auto& [serviceStart, completion] : laneIntervals[laneIndex]) {
            if (serviceStart < previousCompletion) {
                return false;
            }
            previousCompletion = completion;
        }
        const std::int64_t availableMinutes = expectedMakespan -
            unavailableMinutes(
                laneIndex,
                simulationStartMinute,
                finalCompletionMinute,
                scenario.configuration
            );
        const double expectedUtilization = availableMinutes > 0
            ? 100.0 * static_cast<double>(laneBusyMinutes[laneIndex]) /
                static_cast<double>(availableMinutes)
            : 0.0;
        if (lane.passengerCount != lanePassengerCounts[laneIndex] ||
            lane.busyMinutes != laneBusyMinutes[laneIndex] ||
            lane.availableMinutes != availableMinutes ||
            std::abs(lane.utilizationPercent - expectedUtilization) > 1e-9) {
            return false;
        }
    }
    return true;
}

std::string csvField(std::string_view value) {
    if (value.find_first_of(",\"\r\n") == std::string_view::npos) {
        return std::string(value);
    }
    std::string escaped;
    escaped.reserve(value.size() + 2U);
    escaped.push_back('"');
    for (const char character : value) {
        if (character == '"') {
            escaped.push_back('"');
        }
        escaped.push_back(character);
    }
    escaped.push_back('"');
    return escaped;
}

std::string doubleString(double value) {
    std::ostringstream result;
    result << std::setprecision(8) << value;
    return result.str();
}

void writeCsvFields(std::ostream& output, const std::vector<std::string>& fields) {
    for (std::size_t index = 0; index < fields.size(); ++index) {
        if (index != 0U) {
            output.put(',');
        }
        output << csvField(fields[index]);
    }
    output.put('\n');
}

void writeJsonString(std::ostream& output, std::string_view value) {
    static constexpr char hexDigits[] = "0123456789abcdef";
    output.put('"');
    for (const unsigned char character : value) {
        switch (character) {
        case '"':
            output << "\\\"";
            break;
        case '\\':
            output << "\\\\";
            break;
        case '\b':
            output << "\\b";
            break;
        case '\f':
            output << "\\f";
            break;
        case '\n':
            output << "\\n";
            break;
        case '\r':
            output << "\\r";
            break;
        case '\t':
            output << "\\t";
            break;
        default:
            if (character < 0x20U) {
                output << "\\u00"
                       << hexDigits[(character >> 4U) & 0x0fU]
                       << hexDigits[character & 0x0fU];
            } else {
                output.put(static_cast<char>(character));
            }
            break;
        }
    }
    output.put('"');
}

void writeRunJson(std::ostream& output, const ScenarioRunResult& run) {
    output << "{\n  \"run_id\": ";
    writeJsonString(output, run.runId);
    output << ",\n  \"strategy\": ";
    writeJsonString(output, toString(run.strategy));
    output << ",\n  \"input_source\": ";
    writeJsonString(output, run.inputSource);
    output << ",\n  \"summary\": {"
           << "\n    \"passengers\": " << run.passengerCount
           << ",\n    \"completed\": " << run.completedPassengerCount
           << ",\n    \"high_risk_passengers\": " << run.highRiskPassengerCount
           << ",\n    \"elevated_risk_passengers\": "
           << run.elevatedRiskPassengerCount
           << ",\n    \"standard_risk_passengers\": "
           << run.standardRiskPassengerCount
           << ",\n    \"mean_wait_minutes\": " << run.meanWaitMinutes
           << ",\n    \"maximum_wait_minutes\": " << run.maximumWaitMinutes
           << ",\n    \"makespan_minutes\": " << run.makespanMinutes
           << ",\n    \"throughput_per_hour\": " << run.throughputPerHour
           << ",\n    \"staffed_lanes\": " << run.staffedLaneCount
           << ",\n    \"staffed_officers\": " << run.staffedOfficerCount
           << ",\n    \"elapsed_microseconds\": " << run.elapsedMicroseconds
           << ",\n    \"estimated_memory_bytes\": " << run.estimatedMemoryBytes
           << "\n  },\n  \"lanes\": [";
    for (std::size_t index = 0; index < run.lanes.size(); ++index) {
        const LaneSimulationMetrics& lane = run.lanes[index];
        output << (index == 0U ? "\n    {" : ",\n    {")
               << "\"lane_id\": ";
        writeJsonString(output, lane.laneId);
        output << ", \"officer_id\": ";
        writeJsonString(output, lane.officerId);
        output << ", \"passengers\": " << lane.passengerCount
               << ", \"busy_minutes\": " << lane.busyMinutes
               << ", \"available_minutes\": " << lane.availableMinutes
               << ", \"utilization_percent\": " << lane.utilizationPercent
               << '}';
    }
    if (!run.lanes.empty()) {
        output << '\n' << "  ";
    }
    output << "],\n  \"passengers\": [";
    for (std::size_t index = 0; index < run.passengers.size(); ++index) {
        const PassengerServiceRecord& passenger = run.passengers[index];
        output << (index == 0U ? "\n    {" : ",\n    {")
               << "\"id\": ";
        writeJsonString(output, passenger.passenger.id);
        output << ", \"name\": ";
        writeJsonString(output, passenger.passenger.name);
        output << ", \"risk_category\": ";
        writeJsonString(output, toString(passenger.riskCategory));
        output << ", \"priority\": " << passenger.passenger.priority
               << ", \"arrival_minute\": " << passenger.passenger.arrivalMinute
               << ", \"service_start_minute\": " << passenger.serviceStartMinute
               << ", \"completion_minute\": " << passenger.completionMinute
               << ", \"wait_minutes\": " << passenger.waitMinutes
               << ", \"lane_id\": ";
        writeJsonString(output, passenger.laneId);
        output << ", \"officer_id\": ";
        writeJsonString(output, passenger.officerId);
        output << '}';
    }
    if (!run.passengers.empty()) {
        output << '\n' << "  ";
    }
    output << "]\n}";
}

void writeCsvRun(
    std::ostream& output,
    const ScenarioRunResult& run,
    const std::string& scenarioName
) {
    for (const LaneSimulationMetrics& lane : run.lanes) {
        std::vector<std::string> fields(41U);
        fields[0] = "lane";
        fields[1] = run.runId;
        fields[2] = scenarioName;
        fields[3] = toString(run.strategy);
        fields[4] = run.inputSource;
        fields[15] = std::to_string(run.passengerCount);
        fields[16] = std::to_string(run.completedPassengerCount);
        fields[17] = doubleString(run.meanWaitMinutes);
        fields[18] = std::to_string(run.maximumWaitMinutes);
        fields[19] = std::to_string(run.makespanMinutes);
        fields[20] = doubleString(run.throughputPerHour);
        fields[21] = lane.laneId;
        fields[22] = lane.officerId;
        fields[23] = std::to_string(lane.passengerCount);
        fields[24] = std::to_string(lane.busyMinutes);
        fields[25] = std::to_string(lane.availableMinutes);
        fields[26] = doubleString(lane.utilizationPercent);
        fields[27] = std::to_string(run.elapsedMicroseconds);
        fields[28] = std::to_string(run.estimatedMemoryBytes);
        fields[29] = std::to_string(run.staffedLaneCount);
        fields[30] = std::to_string(run.staffedOfficerCount);
        fields[32] = std::to_string(run.highRiskPassengerCount);
        fields[33] = std::to_string(run.elevatedRiskPassengerCount);
        fields[34] = std::to_string(run.standardRiskPassengerCount);
        writeCsvFields(output, fields);
    }
    for (const PassengerServiceRecord& passenger : run.passengers) {
        const auto lane = std::find_if(
            run.lanes.begin(),
            run.lanes.end(),
            [&passenger](const LaneSimulationMetrics& candidate) {
                return candidate.laneId == passenger.laneId;
            });
        if (lane == run.lanes.end()) {
            throw std::logic_error("Scenario result references an unknown lane.");
        }
        std::vector<std::string> fields(41U);
        fields[0] = "passenger";
        fields[1] = run.runId;
        fields[2] = scenarioName;
        fields[3] = toString(run.strategy);
        fields[4] = run.inputSource;
        fields[5] = passenger.passenger.id;
        fields[6] = passenger.passenger.name;
        fields[7] = toString(passenger.riskCategory);
        fields[8] = std::to_string(passenger.passenger.priority);
        fields[9] = std::to_string(passenger.passenger.arrivalMinute);
        fields[10] = std::to_string(passenger.serviceStartMinute);
        fields[11] = std::to_string(passenger.completionMinute);
        fields[12] = std::to_string(passenger.waitMinutes);
        fields[13] = passenger.laneId;
        fields[14] = passenger.officerId;
        fields[15] = std::to_string(run.passengerCount);
        fields[16] = std::to_string(run.completedPassengerCount);
        fields[17] = doubleString(run.meanWaitMinutes);
        fields[18] = std::to_string(run.maximumWaitMinutes);
        fields[19] = std::to_string(run.makespanMinutes);
        fields[20] = doubleString(run.throughputPerHour);
        fields[21] = lane->laneId;
        fields[22] = lane->officerId;
        fields[23] = std::to_string(lane->passengerCount);
        fields[24] = std::to_string(lane->busyMinutes);
        fields[25] = std::to_string(lane->availableMinutes);
        fields[26] = doubleString(lane->utilizationPercent);
        fields[27] = std::to_string(run.elapsedMicroseconds);
        fields[28] = std::to_string(run.estimatedMemoryBytes);
        fields[29] = std::to_string(run.staffedLaneCount);
        fields[30] = std::to_string(run.staffedOfficerCount);
        fields[32] = std::to_string(run.highRiskPassengerCount);
        fields[33] = std::to_string(run.elevatedRiskPassengerCount);
        fields[34] = std::to_string(run.standardRiskPassengerCount);
        writeCsvFields(output, fields);
    }
}

} // namespace

const char* toString(AssignmentStrategy strategy) noexcept {
    switch (strategy) {
    case AssignmentStrategy::ShortestWorkload:
        return "shortest-workload";
    case AssignmentStrategy::RoundRobin:
        return "round-robin";
    }
    return "unknown";
}

const char* toString(PassengerRiskCategory category) noexcept {
    switch (category) {
    case PassengerRiskCategory::High:
        return "high";
    case PassengerRiskCategory::Elevated:
        return "elevated";
    case PassengerRiskCategory::Standard:
        return "standard";
    }
    return "unknown";
}

PassengerRiskCategory riskCategoryForPriority(int priority) {
    if (priority < 1 || priority > 3) {
        throw std::invalid_argument("Scenario priority must be 1, 2, or 3.");
    }
    if (priority == 1) {
        return PassengerRiskCategory::High;
    }
    if (priority == 2) {
        return PassengerRiskCategory::Elevated;
    }
    return PassengerRiskCategory::Standard;
}

int effectivePriorityRank(
    PassengerRiskCategory category,
    std::int64_t waitedMinutes,
    std::int64_t agingIntervalMinutes
) {
    if (waitedMinutes < 0) {
        throw std::invalid_argument("Passenger wait time must not be negative.");
    }
    if (agingIntervalMinutes <= 0) {
        throw std::invalid_argument("Priority aging interval must be positive.");
    }

    int rank = 0;
    switch (category) {
    case PassengerRiskCategory::High:
        rank = 0;
        break;
    case PassengerRiskCategory::Elevated:
        rank = 1;
        break;
    case PassengerRiskCategory::Standard:
        rank = 2;
        break;
    default:
        throw std::invalid_argument("Unknown passenger risk category.");
    }
    const std::int64_t promotions = waitedMinutes / agingIntervalMinutes;
    return promotions >= rank ? 0 : rank - static_cast<int>(promotions);
}

AirportScenario AirportScenarioRunner::loadCsv(
    std::istream& input,
    std::string scenarioName,
    ScenarioConfiguration configuration,
    std::string inputSource
) {
    AirportScenario scenario{
        std::move(scenarioName),
        std::move(inputSource),
        std::move(configuration),
        {},
        false
    };
    std::string line;
    if (!std::getline(input, line)) {
        throw std::invalid_argument("Passenger CSV is empty and has no header.");
    }
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    const std::vector<std::string> expectedHeader{
        "passenger_id",
        "name",
        "arrival_minute",
        "screening_duration_minutes",
        "priority"
    };
    std::vector<std::string> extendedHeader = expectedHeader;
    extendedHeader.emplace_back("eligible_lanes");
    const std::vector<std::string> header = parseCsvLine(line);
    const bool hasEligibilityColumn = header == extendedHeader;
    if (header != expectedHeader && !hasEligibilityColumn) {
        throw std::invalid_argument(
            "Passenger CSV header must be: passenger_id,name,arrival_minute,"
            "screening_duration_minutes,priority[,eligible_lanes]."
        );
    }

    std::size_t lineNumber = 1U;
    while (std::getline(input, line)) {
        ++lineNumber;
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (isBlank(line)) {
            continue;
        }
        try {
            const std::vector<std::string> fields = parseCsvLine(line);
            const std::size_t expectedFieldCount = hasEligibilityColumn ? 6U : 5U;
            if (fields.size() != expectedFieldCount) {
                throw std::invalid_argument(
                    "CSV record must match the number of columns in its header."
                );
            }
            ScenarioPassenger passenger{
                fields[0],
                fields[1],
                parseInteger(fields[2], "arrival_minute"),
                parseInteger(fields[3], "screening_duration_minutes"),
                parseInteger(fields[4], "priority"),
                {}
            };
            if (hasEligibilityColumn) {
                passenger.eligibleLaneIndices = parseEligibleLanes(fields[5]);
            }
            scenario.passengers.push_back(std::move(passenger));
        } catch (const std::invalid_argument& error) {
            throw std::invalid_argument(
                "Passenger CSV line " + std::to_string(lineNumber) + ": " + error.what()
            );
        }
    }
    if (input.bad()) {
        throw std::runtime_error("Failed while reading passenger CSV.");
    }
    std::stable_sort(
        scenario.passengers.begin(),
        scenario.passengers.end(),
        [](const ScenarioPassenger& left, const ScenarioPassenger& right) {
            return left.arrivalMinute < right.arrivalMinute;
        }
    );
    try {
        validateScenario(scenario);
    } catch (const std::invalid_argument& error) {
        throw std::invalid_argument(
            "Passenger CSV validation failed: " + std::string(error.what())
        );
    }
    return scenario;
}

AirportScenario AirportScenarioRunner::loadCsvFile(
    const std::filesystem::path& path,
    std::string scenarioName,
    ScenarioConfiguration configuration
) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("Could not open passenger CSV: " + path.string());
    }
    return loadCsv(input, std::move(scenarioName), std::move(configuration), path.string());
}

AirportScenario AirportScenarioRunner::makeSyntheticScenario(
    std::string scenarioName,
    std::size_t passengerCount,
    ScenarioConfiguration configuration,
    SyntheticArrivalPattern pattern
) {
    if (passengerCount / 2U >
        static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::length_error("Synthetic scenario arrival times exceed the supported range.");
    }
    AirportScenario scenario{
        std::move(scenarioName),
        pattern == SyntheticArrivalPattern::SynchronizedBurst
            ? "deterministic synthetic; all passengers arrive at minute 0"
            : "deterministic synthetic; periodic arrivals",
        std::move(configuration),
        {},
        true
    };
    scenario.passengers.reserve(passengerCount);
    for (std::size_t index = 0; index < passengerCount; ++index) {
        const std::string number = std::to_string(index + 1U);
        std::string id = "SYN-";
        id.append(8U - std::min<std::size_t>(8U, number.size()), '0');
        id += number;
        scenario.passengers.push_back({
            std::move(id),
            "Synthetic passenger " + number,
            pattern == SyntheticArrivalPattern::SynchronizedBurst
                ? 0
                : static_cast<int>(index / 2U),
            2 + static_cast<int>((index * 17U) % 7U),
            1 + static_cast<int>(index % 3U)
        });
    }
    validateScenario(scenario);
    return scenario;
}

ScenarioRunResult AirportScenarioRunner::runStrategy(
    const AirportScenario& scenario,
    AssignmentStrategy strategy
) {
    AirportScenario orderedScenario = scenario;
    std::stable_sort(
        orderedScenario.passengers.begin(),
        orderedScenario.passengers.end(),
        [](const ScenarioPassenger& left, const ScenarioPassenger& right) {
            return left.arrivalMinute < right.arrivalMinute;
        }
    );
    validateScenario(orderedScenario);
    ScenarioRunResult result = runScenario(orderedScenario, strategy);
    if (!resultsCoverScenario(result, orderedScenario)) {
        throw std::logic_error("Scenario correctness validation failed.");
    }
    return result;
}

ScenarioComparison AirportScenarioRunner::compareStrategies(
    const AirportScenario& scenario
) {
    AirportScenario orderedScenario = scenario;
    std::stable_sort(
        orderedScenario.passengers.begin(),
        orderedScenario.passengers.end(),
        [](const ScenarioPassenger& left, const ScenarioPassenger& right) {
            return left.arrivalMinute < right.arrivalMinute;
        }
    );
    validateScenario(orderedScenario);
    if (orderedScenario.configuration.laneCount >= kMaximumStaffedResources ||
        orderedScenario.configuration.officerCount >= kMaximumStaffedResources) {
        throw std::length_error(
            "Cannot compare an additional-lane what-if at the 128-resource limit."
        );
    }

    ScenarioRunResult shortest = runStrategy(
        orderedScenario,
        AssignmentStrategy::ShortestWorkload
    );
    ScenarioRunResult roundRobin = runStrategy(
        orderedScenario,
        AssignmentStrategy::RoundRobin
    );

    AirportScenario extraLaneScenario = orderedScenario;
    ++extraLaneScenario.configuration.laneCount;
    ++extraLaneScenario.configuration.officerCount;
    ScenarioRunResult additionalLane = runStrategy(
        extraLaneScenario,
        AssignmentStrategy::ShortestWorkload
    );

    const ScenarioRunResult& recommended =
        shortest.meanWaitMinutes < roundRobin.meanWaitMinutes ||
        (shortest.meanWaitMinutes == roundRobin.meanWaitMinutes &&
         shortest.maximumWaitMinutes <= roundRobin.maximumWaitMinutes)
            ? shortest
            : roundRobin;
    std::ostringstream resourceRecommendation;
    if (additionalLane.meanWaitMinutes < shortest.meanWaitMinutes) {
        resourceRecommendation
            << "Model one additional staffed lane/officer: mean wait changes from "
            << std::fixed << std::setprecision(2) << shortest.meanWaitMinutes
            << " to " << additionalLane.meanWaitMinutes << " minutes.";
    } else {
        resourceRecommendation
            << "The one-additional-staffed-lane what-if does not lower mean wait "
            << "for this workload ("
            << std::fixed << std::setprecision(2) << shortest.meanWaitMinutes
            << " minutes baseline, " << additionalLane.meanWaitMinutes
            << " minutes projected).";
    }
    return {
        orderedScenario.name,
        orderedScenario.inputSource,
        orderedScenario.syntheticInput,
        orderedScenario.configuration.outages,
        orderedScenario.configuration.officerOutages,
        toString(recommended.strategy),
        resourceRecommendation.str(),
        orderedScenario.configuration.laneCount,
        extraLaneScenario.configuration.laneCount,
        shortest.meanWaitMinutes,
        additionalLane.meanWaitMinutes,
        std::move(shortest),
        std::move(roundRobin),
        std::move(additionalLane)
    };
}

StaffingEvaluation AirportScenarioRunner::evaluateStaffing(
    const AirportScenario& scenario,
    const StaffingEvaluationOptions& options
) {
    AirportScenario orderedScenario = scenario;
    std::stable_sort(
        orderedScenario.passengers.begin(),
        orderedScenario.passengers.end(),
        [](const ScenarioPassenger& left, const ScenarioPassenger& right) {
            return left.arrivalMinute < right.arrivalMinute;
        }
    );
    validateScenario(orderedScenario);
    if (options.minimumLaneCount == 0U ||
        options.minimumLaneCount > options.maximumLaneCount ||
        options.maximumLaneCount > kMaximumStaffedResources) {
        throw std::invalid_argument(
            "Staffing evaluation lane bounds must be ordered within 1..128."
        );
    }
    const auto validateTarget = [](const std::optional<double>& target,
                                   const char* name) {
        if (target && (!std::isfinite(*target) || *target < 0.0)) {
            throw std::invalid_argument(
                std::string(name) + " must be finite and nonnegative."
            );
        }
    };
    validateTarget(options.maximumMeanWaitMinutes, "Maximum mean wait");
    validateTarget(options.minimumThroughputPerHour, "Minimum throughput");
    if (options.maximumPassengerWaitMinutes &&
        *options.maximumPassengerWaitMinutes < 0) {
        throw std::invalid_argument("Maximum passenger wait must be nonnegative.");
    }

    StaffingEvaluation evaluation;
    for (std::size_t laneCount = options.minimumLaneCount;
         laneCount <= options.maximumLaneCount;
         ++laneCount) {
        if (laneCount > std::numeric_limits<std::uint64_t>::max() /
                std::max<std::uint64_t>(1U, options.costPerLane) ||
            laneCount > std::numeric_limits<std::uint64_t>::max() /
                std::max<std::uint64_t>(1U, options.costPerOfficer)) {
            throw std::overflow_error("Staffing resource cost exceeds the supported range.");
        }
        const std::uint64_t laneCost =
            static_cast<std::uint64_t>(laneCount) * options.costPerLane;
        const std::uint64_t officerCost =
            static_cast<std::uint64_t>(laneCount) * options.costPerOfficer;
        if (laneCost > std::numeric_limits<std::uint64_t>::max() - officerCost) {
            throw std::overflow_error("Staffing resource cost exceeds the supported range.");
        }
        const std::uint64_t cost = laneCost + officerCost;
        if (options.maximumCost && cost > *options.maximumCost) {
            continue;
        }

        AirportScenario candidate = orderedScenario;
        candidate.configuration.laneCount = laneCount;
        candidate.configuration.officerCount = laneCount;
        std::erase_if(
            candidate.configuration.outages,
            [laneCount](const LaneOutage& outage) {
                return outage.laneIndex >= laneCount;
            }
        );
        std::erase_if(
            candidate.configuration.officerOutages,
            [laneCount](const OfficerOutage& outage) {
                return outage.officerIndex >= laneCount;
            }
        );
        bool allPassengersHaveEligibleLanes = true;
        for (ScenarioPassenger& passenger : candidate.passengers) {
            if (passenger.eligibleLaneIndices.empty()) {
                continue;
            }
            std::erase_if(
                passenger.eligibleLaneIndices,
                [laneCount](std::size_t eligibleLane) {
                    return eligibleLane >= laneCount;
                }
            );
            if (passenger.eligibleLaneIndices.empty()) {
                allPassengersHaveEligibleLanes = false;
                break;
            }
        }
        if (!allPassengersHaveEligibleLanes) {
            continue;
        }
        validateScenario(candidate);

        ScenarioRunResult run = runStrategy(
            candidate,
            AssignmentStrategy::ShortestWorkload
        );
        const bool meetsTargets =
            (!options.maximumMeanWaitMinutes ||
             run.meanWaitMinutes <= *options.maximumMeanWaitMinutes) &&
            (!options.maximumPassengerWaitMinutes ||
             run.maximumWaitMinutes <= *options.maximumPassengerWaitMinutes) &&
            (!options.minimumThroughputPerHour ||
             run.throughputPerHour >= *options.minimumThroughputPerHour);
        const std::size_t candidateIndex = evaluation.candidates.size();
        evaluation.candidates.push_back({
            laneCount,
            laneCount,
            cost,
            meetsTargets,
            std::move(run)
        });
        if (!meetsTargets) {
            continue;
        }
        if (!evaluation.recommendedCandidateIndex) {
            evaluation.recommendedCandidateIndex = candidateIndex;
            continue;
        }
        const StaffingCandidateResult& current =
            evaluation.candidates[*evaluation.recommendedCandidateIndex];
        const StaffingCandidateResult& proposed =
            evaluation.candidates[candidateIndex];
        if (proposed.resourceCost < current.resourceCost ||
            (proposed.resourceCost == current.resourceCost &&
             (proposed.run.meanWaitMinutes < current.run.meanWaitMinutes ||
              (proposed.run.meanWaitMinutes == current.run.meanWaitMinutes &&
               (proposed.run.maximumWaitMinutes < current.run.maximumWaitMinutes ||
                (proposed.run.maximumWaitMinutes == current.run.maximumWaitMinutes &&
                 proposed.laneCount < current.laneCount)))))) {
            evaluation.recommendedCandidateIndex = candidateIndex;
        }
    }
    return evaluation;
}

void AirportScenarioRunner::writeCsv(
    std::ostream& output,
    const ScenarioComparison& comparison
) {
    output << "record_type,run_id,scenario,strategy,input_source,passenger_id,"
              "passenger_name,risk_category,priority,arrival_minute,"
              "service_start_minute,completion_minute,wait_minutes,lane_id,"
              "officer_id,passenger_count,completed_count,mean_wait_minutes,"
                    "maximum_wait_minutes,makespan_minutes,throughput_per_hour,"
                    "lane_id,lane_officer_id,lane_passenger_count,lane_busy_minutes,"
                    "lane_available_minutes,lane_utilization_percent,elapsed_microseconds,"
                    "estimated_memory_bytes,staffed_lanes,staffed_officers,"
                    "recommendation,high_risk_passengers,elevated_risk_passengers,"
                    "standard_risk_passengers,outage_lane_index,outage_start_minute,"
                    "outage_end_minute,officer_index,officer_outage_start_minute,"
                    "officer_outage_end_minute\n";
    const ScenarioRunResult* runs[]{
        &comparison.shortestWorkload,
        &comparison.roundRobin,
        &comparison.additionalLaneWhatIf
    };
    for (const ScenarioRunResult* run : runs) {
        std::vector<std::string> fields(41U);
        fields[0] = "summary";
        fields[1] = run->runId;
        fields[2] = comparison.scenarioName;
        fields[3] = toString(run->strategy);
        fields[4] = run->inputSource;
        fields[15] = std::to_string(run->passengerCount);
        fields[16] = std::to_string(run->completedPassengerCount);
        fields[17] = doubleString(run->meanWaitMinutes);
        fields[18] = std::to_string(run->maximumWaitMinutes);
        fields[19] = std::to_string(run->makespanMinutes);
        fields[20] = doubleString(run->throughputPerHour);
        fields[27] = std::to_string(run->elapsedMicroseconds);
        fields[28] = std::to_string(run->estimatedMemoryBytes);
        fields[29] = std::to_string(run->staffedLaneCount);
        fields[30] = std::to_string(run->staffedOfficerCount);
        fields[32] = std::to_string(run->highRiskPassengerCount);
        fields[33] = std::to_string(run->elevatedRiskPassengerCount);
        fields[34] = std::to_string(run->standardRiskPassengerCount);
        writeCsvFields(output, fields);
        writeCsvRun(output, *run, comparison.scenarioName);
    }
    std::vector<std::string> recommendation(41U);
    recommendation[0] = "recommendation";
    recommendation[2] = comparison.scenarioName;
    recommendation[3] = comparison.recommendedStrategy;
    recommendation[31] = comparison.resourceRecommendation;
    writeCsvFields(output, recommendation);
    for (const LaneOutage& outage : comparison.outages) {
        std::vector<std::string> fields(41U);
        fields[0] = "outage";
        fields[2] = comparison.scenarioName;
        fields[4] = comparison.inputSource;
        fields[13] = "Lane-" + std::to_string(outage.laneIndex + 1U);
        fields[14] = "Officer-" + std::to_string(outage.laneIndex + 1U);
        fields[35] = std::to_string(outage.laneIndex);
        fields[36] = std::to_string(outage.startMinute);
        fields[37] = std::to_string(outage.endMinute);
        writeCsvFields(output, fields);
    }
    for (const OfficerOutage& outage : comparison.officerOutages) {
        std::vector<std::string> fields(41U);
        fields[0] = "officer_outage";
        fields[2] = comparison.scenarioName;
        fields[4] = comparison.inputSource;
        fields[14] = "Officer-" + std::to_string(outage.officerIndex + 1U);
        fields[38] = std::to_string(outage.officerIndex);
        fields[39] = std::to_string(outage.startMinute);
        fields[40] = std::to_string(outage.endMinute);
        writeCsvFields(output, fields);
    }
    if (!output) {
        throw std::runtime_error("Failed while writing scenario CSV.");
    }
}

void AirportScenarioRunner::writeJson(
    std::ostream& output,
    const ScenarioComparison& comparison
) {
    output << std::setprecision(8) << "{\n  \"scenario\": ";
    writeJsonString(output, comparison.scenarioName);
    output << ",\n  \"correctness_verified\": true"
           << ",\n  \"input_source\": ";
    writeJsonString(output, comparison.inputSource);
    output << ",\n  \"synthetic_input\": "
           << (comparison.syntheticInput ? "true" : "false")
           << ",\n  \"lane_outages\": [";
    for (std::size_t index = 0; index < comparison.outages.size(); ++index) {
        const LaneOutage& outage = comparison.outages[index];
        output << (index == 0U ? "\n    {" : ",\n    {")
               << "\"lane_index\": " << outage.laneIndex
               << ", \"start_minute\": " << outage.startMinute
               << ", \"end_minute\": " << outage.endMinute << '}';
    }
    if (!comparison.outages.empty()) {
        output << '\n' << "  ";
    }
    output << "],\n  \"officer_outages\": [";
    for (std::size_t index = 0;
         index < comparison.officerOutages.size();
         ++index) {
        const OfficerOutage& outage = comparison.officerOutages[index];
        output << (index == 0U ? "\n    {" : ",\n    {")
               << "\"officer_index\": " << outage.officerIndex
               << ", \"start_minute\": " << outage.startMinute
               << ", \"end_minute\": " << outage.endMinute << '}';
    }
    if (!comparison.officerOutages.empty()) {
        output << '\n' << "  ";
    }
    output << "],\n  \"recommended_strategy\": ";
    writeJsonString(output, comparison.recommendedStrategy);
    output << ",\n  \"resource_recommendation\": ";
    writeJsonString(output, comparison.resourceRecommendation);
    output << ",\n  \"staffing_what_if\": {"
           << "\n    \"baseline_lanes\": " << comparison.baselineLaneCount
           << ",\n    \"projected_lanes\": " << comparison.projectedLaneCount
           << ",\n    \"baseline_mean_wait_minutes\": "
           << comparison.baselineMeanWaitMinutes
           << ",\n    \"projected_mean_wait_minutes\": "
           << comparison.projectedMeanWaitMinutes << "\n  },\n  \"runs\": [\n";
    writeRunJson(output, comparison.shortestWorkload);
    output << ",\n";
    writeRunJson(output, comparison.roundRobin);
    output << ",\n";
    writeRunJson(output, comparison.additionalLaneWhatIf);
    output << "\n  ]\n}\n";
    if (!output) {
        throw std::runtime_error("Failed while writing scenario JSON.");
    }
}

void AirportScenarioRunner::exportCsv(
    const std::filesystem::path& path,
    const ScenarioComparison& comparison
) {
    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("Could not open scenario CSV for writing: " + path.string());
    }
    writeCsv(output, comparison);
    output.flush();
    if (!output) {
        throw std::runtime_error("Failed to finish writing scenario CSV: " + path.string());
    }
}

void AirportScenarioRunner::exportJson(
    const std::filesystem::path& path,
    const ScenarioComparison& comparison
) {
    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("Could not open scenario JSON for writing: " + path.string());
    }
    writeJson(output, comparison);
    output.flush();
    if (!output) {
        throw std::runtime_error("Failed to finish writing scenario JSON: " + path.string());
    }
}

} // namespace project31
