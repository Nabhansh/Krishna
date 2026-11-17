#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

namespace project31 {

enum class AssignmentStrategy {
    ShortestWorkload,
    RoundRobin
};

enum class PassengerRiskCategory {
    High,
    Elevated,
    Standard
};

enum class SyntheticArrivalPattern {
    Steady,
    SynchronizedBurst
};

struct ScenarioPassenger {
    std::string id;
    std::string name;
    int arrivalMinute;
    int screeningDurationMinutes;
    int priority;
    std::vector<std::size_t> eligibleLaneIndices{};
};

struct LaneOutage {
    std::size_t laneIndex;
    int startMinute;
    int endMinute;
};

struct OfficerOutage {
    std::size_t officerIndex;
    int startMinute;
    int endMinute;
};

struct ScenarioConfiguration {
    std::size_t laneCount{2};
    std::size_t officerCount{2};
    std::vector<LaneOutage> outages;
    std::int64_t priorityAgingIntervalMinutes{15};
    std::vector<OfficerOutage> officerOutages;
};

struct AirportScenario {
    std::string name;
    std::string inputSource;
    ScenarioConfiguration configuration;
    std::vector<ScenarioPassenger> passengers;
    bool syntheticInput{false};
};

struct PassengerServiceRecord {
    ScenarioPassenger passenger;
    PassengerRiskCategory riskCategory;
    std::string laneId;
    std::string officerId;
    std::int64_t serviceStartMinute;
    std::int64_t completionMinute;
    std::int64_t waitMinutes;
};

struct LaneSimulationMetrics {
    std::string laneId;
    std::string officerId;
    std::size_t passengerCount;
    std::int64_t busyMinutes;
    std::int64_t availableMinutes;
    double utilizationPercent;
};

struct ScenarioRunResult {
    std::string scenarioName;
    std::string inputSource;
    std::string runId;
    AssignmentStrategy strategy;
    std::size_t passengerCount;
    std::size_t completedPassengerCount;
    std::size_t highRiskPassengerCount;
    std::size_t elevatedRiskPassengerCount;
    std::size_t standardRiskPassengerCount;
    std::int64_t totalWaitMinutes;
    double meanWaitMinutes;
    std::int64_t maximumWaitMinutes;
    std::int64_t makespanMinutes;
    double throughputPerHour;
    std::size_t staffedLaneCount;
    std::size_t staffedOfficerCount;
    std::uint64_t elapsedMicroseconds;
    std::size_t estimatedMemoryBytes;
    std::vector<LaneSimulationMetrics> lanes;
    std::vector<PassengerServiceRecord> passengers;
};

struct ScenarioComparison {
    std::string scenarioName;
    std::string inputSource;
    bool syntheticInput;
    std::vector<LaneOutage> outages;
    std::vector<OfficerOutage> officerOutages;
    std::string recommendedStrategy;
    std::string resourceRecommendation;
    std::size_t baselineLaneCount;
    std::size_t projectedLaneCount;
    double baselineMeanWaitMinutes;
    double projectedMeanWaitMinutes;
    ScenarioRunResult shortestWorkload;
    ScenarioRunResult roundRobin;
    ScenarioRunResult additionalLaneWhatIf;
};

struct StaffingEvaluationOptions {
    std::size_t minimumLaneCount{1U};
    std::size_t maximumLaneCount{128U};
    std::uint64_t costPerLane{1U};
    std::uint64_t costPerOfficer{1U};
    std::optional<std::uint64_t> maximumCost;
    std::optional<double> maximumMeanWaitMinutes;
    std::optional<std::int64_t> maximumPassengerWaitMinutes;
    std::optional<double> minimumThroughputPerHour;
};

struct StaffingCandidateResult {
    std::size_t laneCount;
    std::size_t officerCount;
    std::uint64_t resourceCost;
    bool meetsTargets;
    ScenarioRunResult run;
};

struct StaffingEvaluation {
    std::vector<StaffingCandidateResult> candidates;
    std::optional<std::size_t> recommendedCandidateIndex;
};

class AirportScenarioRunner {
public:
    [[nodiscard]] static AirportScenario loadCsv(
        std::istream& input,
        std::string scenarioName,
        ScenarioConfiguration configuration,
        std::string inputSource = "CSV stream"
    );
    [[nodiscard]] static AirportScenario loadCsvFile(
        const std::filesystem::path& path,
        std::string scenarioName,
        ScenarioConfiguration configuration
    );
    [[nodiscard]] static AirportScenario makeSyntheticScenario(
        std::string scenarioName,
        std::size_t passengerCount,
        ScenarioConfiguration configuration,
        SyntheticArrivalPattern pattern = SyntheticArrivalPattern::Steady
    );
    [[nodiscard]] static ScenarioRunResult runStrategy(
        const AirportScenario& scenario,
        AssignmentStrategy strategy
    );
    [[nodiscard]] static ScenarioComparison compareStrategies(
        const AirportScenario& scenario
    );
    [[nodiscard]] static StaffingEvaluation evaluateStaffing(
        const AirportScenario& scenario,
        const StaffingEvaluationOptions& options
    );
    static void writeCsv(std::ostream& output, const ScenarioComparison& comparison);
    static void writeJson(std::ostream& output, const ScenarioComparison& comparison);
    static void exportCsv(
        const std::filesystem::path& path,
        const ScenarioComparison& comparison
    );
    static void exportJson(
        const std::filesystem::path& path,
        const ScenarioComparison& comparison
    );
};

[[nodiscard]] const char* toString(AssignmentStrategy strategy) noexcept;
[[nodiscard]] const char* toString(PassengerRiskCategory category) noexcept;
[[nodiscard]] PassengerRiskCategory riskCategoryForPriority(int priority);
[[nodiscard]] int effectivePriorityRank(
    PassengerRiskCategory category,
    std::int64_t waitedMinutes,
    std::int64_t agingIntervalMinutes
);

} // namespace project31
