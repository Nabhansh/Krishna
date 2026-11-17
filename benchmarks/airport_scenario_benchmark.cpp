#include "simulator/AirportScenario.hpp"

#include <algorithm>
#include <array>
#include <cerrno>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#if defined(_WIN32)
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <Psapi.h>
#else
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

using namespace project31;

constexpr std::size_t kRepetitions = 3U;

struct BenchmarkDataset {
    std::string name;
    std::size_t passengerCount;
    std::size_t laneCount;
    SyntheticArrivalPattern arrivalPattern;
};

struct PassengerMeasurement {
    std::string id;
    std::string laneId;
    std::string officerId;
    std::int64_t arrivalMinute;
    std::int64_t serviceStartMinute;
    std::int64_t completionMinute;
    std::int64_t waitMinutes;
};

struct LaneMeasurement {
    std::string laneId;
    std::string officerId;
    std::size_t passengerCount;
    std::int64_t busyMinutes;
    std::int64_t availableMinutes;
    double utilizationPercent;
};

struct WorkerMeasurement {
    std::uint64_t runtimeMicroseconds{};
    std::uint64_t peakMemoryKiB{};
    std::size_t estimatedResultBytes{};
    std::size_t passengerCount{};
    std::size_t completedCount{};
    double meanWaitMinutes{};
    std::int64_t maximumWaitMinutes{};
    std::int64_t makespanMinutes{};
    double throughputPerHour{};
    std::vector<LaneMeasurement> lanes;
    std::vector<PassengerMeasurement> passengers;
};

AirportScenario makeBenchmarkScenario(const BenchmarkDataset& dataset) {
    if (dataset.name == "assignment-divergence") {
        return {
            "Assignment divergence",
            "deterministic synthetic; strategy-divergence workload",
            {2U, 2U, {}, 1000, {}},
            {
                {"A", "A", 0, 10, 3, {}},
                {"B", "B", 1, 9, 3, {}},
                {"C", "C", 10, 1, 3, {}}
            },
            true
        };
    }
    return AirportScenarioRunner::makeSyntheticScenario(
        "Benchmark " + dataset.name,
        dataset.passengerCount,
        {dataset.laneCount, dataset.laneCount, {}, 15, {}},
        dataset.arrivalPattern
    );
}

std::size_t parseSize(std::string_view value, const char* fieldName) {
    std::size_t result{};
    const auto [end, error] = std::from_chars(
        value.data(),
        value.data() + value.size(),
        result
    );
    if (error != std::errc{} || end != value.data() + value.size()) {
        throw std::invalid_argument(std::string("Invalid worker ") + fieldName + ".");
    }
    return result;
}

int runWorker(int argc, char** argv) {
    if (argc != 6) {
        throw std::invalid_argument("Benchmark worker received invalid arguments.");
    }
    const std::string datasetName(argv[2]);
    const std::size_t passengerCount = parseSize(argv[3], "passenger count");
    const std::string strategyName(argv[4]);
    AssignmentStrategy strategy;
    if (strategyName == "shortest") {
        strategy = AssignmentStrategy::ShortestWorkload;
    } else if (strategyName == "round-robin") {
        strategy = AssignmentStrategy::RoundRobin;
    } else {
        throw std::invalid_argument("Benchmark worker received an unknown strategy.");
    }
    const std::string workerMode(argv[5]);
    if (workerMode != "--preflight" && workerMode != "--measure") {
        throw std::invalid_argument("Benchmark worker received an unknown mode.");
    }

    BenchmarkDataset dataset{
        datasetName,
        passengerCount,
        datasetName == "assignment-divergence" ? 2U : 6U,
        datasetName == "adversarial-burst"
            ? SyntheticArrivalPattern::SynchronizedBurst
            : SyntheticArrivalPattern::Steady
    };
    const AirportScenario scenario = makeBenchmarkScenario(dataset);
    const ScenarioRunResult run =
        AirportScenarioRunner::runStrategy(scenario, strategy);
    std::cout << "P31RESULT "
              << run.elapsedMicroseconds << ' '
              << run.estimatedMemoryBytes << ' '
              << run.passengerCount << ' '
              << run.completedPassengerCount << ' '
              << std::setprecision(17) << run.meanWaitMinutes << ' '
              << run.maximumWaitMinutes << ' '
              << run.makespanMinutes << ' '
              << run.throughputPerHour << ' '
              << run.lanes.size() << ' '
              << run.passengers.size() << '\n';
    for (const LaneSimulationMetrics& lane : run.lanes) {
        std::cout << "L " << lane.laneId << ' ' << lane.officerId << ' '
                  << lane.passengerCount << ' ' << lane.busyMinutes << ' '
                  << lane.availableMinutes << ' '
                  << std::setprecision(17) << lane.utilizationPercent << '\n';
    }
    for (const PassengerServiceRecord& passenger : run.passengers) {
        std::cout << "P " << passenger.passenger.id << ' '
                  << passenger.laneId << ' ' << passenger.officerId << ' '
                  << passenger.passenger.arrivalMinute << ' '
                  << passenger.serviceStartMinute << ' '
                  << passenger.completionMinute << ' '
                  << passenger.waitMinutes << '\n';
    }
    std::cout.flush();
    if (!std::cout) {
        return 1;
    }
#if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS_EX memoryCounters{};
    memoryCounters.cb = static_cast<DWORD>(sizeof(memoryCounters));
    if (!GetProcessMemoryInfo(
            GetCurrentProcess(),
            reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&memoryCounters),
            static_cast<DWORD>(sizeof(memoryCounters)))) {
        throw std::runtime_error(
            "GetProcessMemoryInfo failed in benchmark worker with error " +
            std::to_string(GetLastError()) + "."
        );
    }
    const auto peakMemoryKiB = static_cast<std::uint64_t>(
        memoryCounters.PeakWorkingSetSize / 1024U
    );
#else
    rusage usage{};
    if (getrusage(RUSAGE_SELF, &usage) != 0 || usage.ru_maxrss < 0) {
        throw std::runtime_error("getrusage(RUSAGE_SELF) failed in benchmark worker.");
    }
    const auto peakMemoryRaw = static_cast<std::uint64_t>(usage.ru_maxrss);
#if defined(__APPLE__)
    const auto peakMemoryKiB = peakMemoryRaw / 1024U;
#else
    const auto peakMemoryKiB = peakMemoryRaw;
#endif
#endif
    std::cout << "R " << peakMemoryKiB << '\n';
    return std::cout ? 0 : 1;
}

#if defined(_WIN32)
class UniqueHandle {
public:
    explicit UniqueHandle(HANDLE value = nullptr) : value_(value) {}
    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;
    ~UniqueHandle() {
        if (value_ != nullptr && value_ != INVALID_HANDLE_VALUE) {
            CloseHandle(value_);
        }
    }
    HANDLE get() const noexcept { return value_; }
    void reset() noexcept {
        if (value_ != nullptr && value_ != INVALID_HANDLE_VALUE) {
            CloseHandle(value_);
        }
        value_ = nullptr;
    }

private:
    HANDLE value_;
};

std::wstring quoteWindowsArgument(std::wstring_view argument) {
    std::wstring quoted = L"\"";
    std::size_t backslashes = 0U;
    for (const wchar_t character : argument) {
        if (character == L'\\') {
            ++backslashes;
        } else if (character == L'"') {
            quoted.append(backslashes * 2U + 1U, L'\\');
            quoted.push_back(L'"');
            backslashes = 0U;
        } else {
            quoted.append(backslashes, L'\\');
            quoted.push_back(character);
            backslashes = 0U;
        }
    }
    quoted.append(backslashes * 2U, L'\\');
    quoted.push_back(L'"');
    return quoted;
}

std::wstring widenAscii(std::string_view value) {
    return std::wstring(value.begin(), value.end());
}
#endif

std::string executeWorker(
    const std::filesystem::path& executable,
    const BenchmarkDataset& dataset,
    AssignmentStrategy strategy,
    bool preflight
) {
#if defined(_WIN32)
    SECURITY_ATTRIBUTES securityAttributes{};
    securityAttributes.nLength =
        static_cast<DWORD>(sizeof(securityAttributes));
    securityAttributes.bInheritHandle = TRUE;
    HANDLE readPipeRaw{};
    HANDLE writePipeRaw{};
    if (!CreatePipe(
            &readPipeRaw,
            &writePipeRaw,
            &securityAttributes,
            0
        )) {
        throw std::runtime_error(
            "Could not create benchmark worker pipe; Windows error " +
            std::to_string(GetLastError()) + "."
        );
    }
    UniqueHandle readPipe(readPipeRaw);
    UniqueHandle writePipe(writePipeRaw);
    if (!SetHandleInformation(readPipe.get(), HANDLE_FLAG_INHERIT, 0)) {
        throw std::runtime_error(
            "Could not configure benchmark worker pipe; Windows error " +
            std::to_string(GetLastError()) + "."
        );
    }

    const std::wstring executableArgument = executable.native();
    const std::wstring workerArgument = L"--worker";
    const std::wstring datasetArgument = widenAscii(dataset.name);
    const std::wstring count = widenAscii(std::to_string(dataset.passengerCount));
    const std::wstring strategyArgument =
        strategy == AssignmentStrategy::ShortestWorkload
            ? L"shortest"
            : L"round-robin";
    const std::wstring modeArgument = preflight ? L"--preflight" : L"--measure";
    std::wstring commandLine =
        quoteWindowsArgument(executableArgument) + L" " +
        quoteWindowsArgument(workerArgument) + L" " +
        quoteWindowsArgument(datasetArgument) + L" " +
        quoteWindowsArgument(count) + L" " +
        quoteWindowsArgument(strategyArgument) + L" " +
        quoteWindowsArgument(modeArgument);

    STARTUPINFOW startupInfo{};
    startupInfo.cb = static_cast<DWORD>(sizeof(startupInfo));
    startupInfo.dwFlags = STARTF_USESTDHANDLES;
    startupInfo.hStdInput = writePipe.get();
    startupInfo.hStdOutput = writePipe.get();
    startupInfo.hStdError = writePipe.get();
    PROCESS_INFORMATION processInfo{};
    if (!CreateProcessW(
            executable.c_str(),
            commandLine.data(),
            nullptr,
            nullptr,
            TRUE,
            CREATE_NO_WINDOW,
            nullptr,
            nullptr,
            &startupInfo,
            &processInfo
        )) {
        throw std::runtime_error(
            "Could not start benchmark worker process; Windows error " +
            std::to_string(GetLastError()) + "."
        );
    }
    UniqueHandle processHandle(processInfo.hProcess);
    UniqueHandle threadHandle(processInfo.hThread);
    writePipe.reset();

    std::string output;
    std::array<char, 16U * 1024U> buffer{};
    while (true) {
        DWORD bytesRead{};
        if (ReadFile(
                readPipe.get(),
                buffer.data(),
                static_cast<DWORD>(buffer.size()),
                &bytesRead,
                nullptr
            )) {
            if (bytesRead == 0U) {
                break;
            }
            output.append(buffer.data(), static_cast<std::size_t>(bytesRead));
        } else if (GetLastError() == ERROR_BROKEN_PIPE) {
            break;
        } else {
            const DWORD error = GetLastError();
            readPipe.reset();
            WaitForSingleObject(processHandle.get(), INFINITE);
            throw std::runtime_error(
                "Failed while reading benchmark worker output; Windows error " +
                std::to_string(error) + "."
            );
        }
    }
    if (WaitForSingleObject(processHandle.get(), INFINITE) != WAIT_OBJECT_0) {
        throw std::runtime_error(
            "Could not collect benchmark worker status; Windows error " +
            std::to_string(GetLastError()) + "."
        );
    }
    DWORD exitCode{};
    if (!GetExitCodeProcess(processHandle.get(), &exitCode)) {
        throw std::runtime_error(
            "Could not read benchmark worker status; Windows error " +
            std::to_string(GetLastError()) + "."
        );
    }
    if (exitCode != 0U) {
        throw std::runtime_error(
            "Benchmark worker failed for " + dataset.name + " (" +
            toString(strategy) + "), exit code " + std::to_string(exitCode) + "."
        );
    }
    return output;
#else
    int pipeDescriptors[2]{};
    if (pipe(pipeDescriptors) != 0) {
        throw std::runtime_error("Could not create benchmark worker pipe.");
    }
    const pid_t child = fork();
    if (child < 0) {
        close(pipeDescriptors[0]);
        close(pipeDescriptors[1]);
        throw std::runtime_error("Could not start benchmark worker process.");
    }
    if (child == 0) {
        close(pipeDescriptors[0]);
        if (dup2(pipeDescriptors[1], STDOUT_FILENO) < 0) {
            _exit(126);
        }
        close(pipeDescriptors[1]);
        std::string executableArgument = executable.string();
        std::string workerArgument = "--worker";
        std::string datasetArgument = dataset.name;
        std::string count = std::to_string(dataset.passengerCount);
        std::string strategyArgument =
            strategy == AssignmentStrategy::ShortestWorkload
                ? "shortest"
                : "round-robin";
        std::string modeArgument = preflight ? "--preflight" : "--measure";
        char* const arguments[]{
            executableArgument.data(),
            workerArgument.data(),
            datasetArgument.data(),
            count.data(),
            strategyArgument.data(),
            modeArgument.data(),
            nullptr
        };
        execv(executable.c_str(), arguments);
        _exit(127);
    }

    close(pipeDescriptors[1]);
    std::string output;
    std::array<char, 16U * 1024U> buffer{};
    while (true) {
        const ssize_t count = read(pipeDescriptors[0], buffer.data(), buffer.size());
        if (count > 0) {
            output.append(buffer.data(), static_cast<std::size_t>(count));
        } else if (count == 0) {
            break;
        } else if (errno != EINTR) {
            close(pipeDescriptors[0]);
            int ignoredStatus{};
            waitpid(child, &ignoredStatus, 0);
            throw std::runtime_error("Failed while reading benchmark worker output.");
        }
    }
    close(pipeDescriptors[0]);
    int status{};
    while (waitpid(child, &status, 0) < 0) {
        if (errno != EINTR) {
            throw std::runtime_error("Could not collect benchmark worker status.");
        }
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        throw std::runtime_error(
            "Benchmark worker failed for " + dataset.name + " (" + toString(strategy) +
            ")."
        );
    }
    return output;
#endif
}

WorkerMeasurement parseWorkerOutput(const std::string& output) {
    std::istringstream input(output);
    WorkerMeasurement measurement;
    std::string marker;
    std::size_t laneCount{};
    std::size_t serviceCount{};
    if (!(input >> marker) || marker != "P31RESULT" ||
        !(input >> measurement.runtimeMicroseconds
                >> measurement.estimatedResultBytes
                >> measurement.passengerCount
                >> measurement.completedCount
                >> measurement.meanWaitMinutes
                >> measurement.maximumWaitMinutes
                >> measurement.makespanMinutes
                >> measurement.throughputPerHour
                >> laneCount
                >> serviceCount)) {
        throw std::runtime_error("Malformed benchmark worker summary.");
    }
    measurement.lanes.reserve(laneCount);
    for (std::size_t index = 0; index < laneCount; ++index) {
        LaneMeasurement lane;
        if (!(input >> marker) || marker != "L" ||
            !(input >> lane.laneId
                    >> lane.officerId
                    >> lane.passengerCount
                    >> lane.busyMinutes
                    >> lane.availableMinutes
                    >> lane.utilizationPercent)) {
            throw std::runtime_error("Malformed benchmark worker lane record.");
        }
        measurement.lanes.push_back(std::move(lane));
    }
    measurement.passengers.reserve(serviceCount);
    for (std::size_t index = 0; index < serviceCount; ++index) {
        PassengerMeasurement passenger;
        if (!(input >> marker) || marker != "P" ||
            !(input >> passenger.id
                    >> passenger.laneId
                    >> passenger.officerId
                    >> passenger.arrivalMinute
                    >> passenger.serviceStartMinute
                    >> passenger.completionMinute
                    >> passenger.waitMinutes)) {
            throw std::runtime_error("Malformed benchmark worker passenger record.");
        }
        measurement.passengers.push_back(std::move(passenger));
    }
    if (!(input >> marker) || marker != "R" || !(input >> measurement.peakMemoryKiB)) {
        throw std::runtime_error("Missing benchmark worker peak-memory record.");
    }
    if (input >> marker) {
        throw std::runtime_error("Unexpected trailing benchmark worker output.");
    }
    return measurement;
}

void validateWorkerMeasurement(
    const WorkerMeasurement& run,
    const AirportScenario& scenario
) {
    if (run.passengerCount != scenario.passengers.size() ||
        run.completedCount != scenario.passengers.size() ||
        run.passengers.size() != scenario.passengers.size() ||
        run.lanes.size() != scenario.configuration.laneCount) {
        throw std::runtime_error("Benchmark worker did not complete its input.");
    }
    std::unordered_map<std::string, const LaneMeasurement*> lanes;
    std::unordered_map<std::string, const ScenarioPassenger*> passengers;
    for (const LaneMeasurement& lane : run.lanes) {
        if (!lanes.emplace(lane.laneId, &lane).second) {
            throw std::runtime_error("Benchmark worker reported a duplicate lane.");
        }
    }
    for (const ScenarioPassenger& passenger : scenario.passengers) {
        passengers.emplace(passenger.id, &passenger);
    }

    std::unordered_map<std::string, std::vector<std::pair<std::int64_t, std::int64_t>>>
        laneIntervals;
    std::unordered_map<std::string, std::vector<std::pair<std::int64_t, std::int64_t>>>
        officerIntervals;
    std::unordered_map<std::string, std::pair<std::size_t, std::int64_t>> laneTotals;
    std::int64_t totalWait{};
    std::int64_t maxWait{};
    std::int64_t lastCompletion{};
    for (const PassengerMeasurement& service : run.passengers) {
        const auto expected = passengers.find(service.id);
        const auto lane = lanes.find(service.laneId);
        if (expected == passengers.end() || lane == lanes.end() ||
            lane->second->officerId != service.officerId ||
            service.serviceStartMinute < expected->second->arrivalMinute ||
            service.completionMinute - service.serviceStartMinute !=
                expected->second->screeningDurationMinutes ||
            service.waitMinutes !=
                service.serviceStartMinute - expected->second->arrivalMinute) {
            throw std::runtime_error("Benchmark worker reported an invalid service.");
        }
        if (!expected->second->eligibleLaneIndices.empty()) {
            const auto laneIndex = static_cast<std::size_t>(
                std::distance(run.lanes.begin(), std::find_if(
                    run.lanes.begin(),
                    run.lanes.end(),
                    [&service](const LaneMeasurement& candidate) {
                        return candidate.laneId == service.laneId;
                    }
                ))
            );
            if (std::find(
                    expected->second->eligibleLaneIndices.begin(),
                    expected->second->eligibleLaneIndices.end(),
                    laneIndex
                ) == expected->second->eligibleLaneIndices.end()) {
                throw std::runtime_error("Benchmark worker violated lane eligibility.");
            }
        }
        laneIntervals[service.laneId].emplace_back(
            service.serviceStartMinute,
            service.completionMinute
        );
        officerIntervals[service.officerId].emplace_back(
            service.serviceStartMinute,
            service.completionMinute
        );
        auto& totals = laneTotals[service.laneId];
        ++totals.first;
        totals.second += expected->second->screeningDurationMinutes;
        totalWait += service.waitMinutes;
        maxWait = std::max(maxWait, service.waitMinutes);
        lastCompletion = std::max(lastCompletion, service.completionMinute);
        for (const LaneOutage& outage : scenario.configuration.outages) {
            if (service.laneId == "Lane-" + std::to_string(outage.laneIndex + 1U) &&
                service.serviceStartMinute < outage.endMinute &&
                outage.startMinute < service.completionMinute) {
                throw std::runtime_error("Benchmark worker scheduled into a lane outage.");
            }
        }
        for (const OfficerOutage& outage : scenario.configuration.officerOutages) {
            if (service.officerId ==
                    "Officer-" + std::to_string(outage.officerIndex + 1U) &&
                service.serviceStartMinute < outage.endMinute &&
                outage.startMinute < service.completionMinute) {
                throw std::runtime_error("Benchmark worker scheduled into an officer outage.");
            }
        }
        passengers.erase(expected);
    }
    if (!passengers.empty()) {
        throw std::runtime_error("Benchmark worker omitted or duplicated passengers.");
    }
    const auto validateIntervals = [](auto& intervals) {
        for (auto& [resource, services] : intervals) {
            (void)resource;
            std::sort(services.begin(), services.end());
            for (std::size_t index = 1U; index < services.size(); ++index) {
                if (services[index].first < services[index - 1U].second) {
                    throw std::runtime_error("Benchmark worker overlapped a resource.");
                }
            }
        }
    };
    validateIntervals(laneIntervals);
    validateIntervals(officerIntervals);
    const std::int64_t startMinute = scenario.passengers.empty()
        ? 0
        : std::min_element(
              scenario.passengers.begin(),
              scenario.passengers.end(),
              [](const auto& left, const auto& right) {
                  return left.arrivalMinute < right.arrivalMinute;
              }
          )->arrivalMinute;
    const std::int64_t makespan = lastCompletion - startMinute;
    const double meanWait = run.passengers.empty()
        ? 0.0
        : static_cast<double>(totalWait) /
            static_cast<double>(run.passengers.size());
    const double throughput = makespan <= 0
        ? 0.0
        : static_cast<double>(run.completedCount) * 60.0 /
            static_cast<double>(makespan);
    if (run.maximumWaitMinutes != maxWait ||
        run.makespanMinutes != makespan ||
        std::abs(run.meanWaitMinutes - meanWait) > 1e-9 ||
        std::abs(run.throughputPerHour - throughput) > 1e-9) {
        throw std::runtime_error("Benchmark worker aggregate metrics are inconsistent.");
    }
    for (const LaneMeasurement& lane : run.lanes) {
        const auto totals = laneTotals[lane.laneId];
        if (lane.passengerCount != totals.first || lane.busyMinutes != totals.second) {
            throw std::runtime_error("Benchmark worker lane metrics are inconsistent.");
        }
    }
}

bool sameSchedule(const WorkerMeasurement& left, const WorkerMeasurement& right) {
    if (left.passengers.size() != right.passengers.size() ||
        left.lanes.size() != right.lanes.size() ||
        left.completedCount != right.completedCount ||
        left.meanWaitMinutes != right.meanWaitMinutes ||
        left.maximumWaitMinutes != right.maximumWaitMinutes ||
        left.makespanMinutes != right.makespanMinutes ||
        left.throughputPerHour != right.throughputPerHour) {
        return false;
    }
    for (std::size_t index = 0; index < left.passengers.size(); ++index) {
        const PassengerMeasurement& a = left.passengers[index];
        const PassengerMeasurement& b = right.passengers[index];
        if (a.id != b.id || a.laneId != b.laneId || a.officerId != b.officerId ||
            a.arrivalMinute != b.arrivalMinute ||
            a.serviceStartMinute != b.serviceStartMinute ||
            a.completionMinute != b.completionMinute ||
            a.waitMinutes != b.waitMinutes) {
            return false;
        }
    }
    return true;
}

std::pair<std::size_t, std::size_t> countScheduleDifferences(
    const WorkerMeasurement& left,
    const WorkerMeasurement& right
) {
    if (left.passengers.size() != right.passengers.size()) {
        throw std::runtime_error(
            "Strategy outputs do not use identical passenger counts."
        );
    }
    std::size_t assignmentChanges = 0U;
    std::size_t intervalChanges = 0U;
    for (std::size_t index = 0; index < left.passengers.size(); ++index) {
        const PassengerMeasurement& a = left.passengers[index];
        const PassengerMeasurement& b = right.passengers[index];
        if (a.id != b.id) {
            throw std::runtime_error(
                "Strategy outputs do not use identical passenger ordering."
            );
        }
        if (a.laneId != b.laneId || a.officerId != b.officerId) {
            ++assignmentChanges;
        }
        if (a.serviceStartMinute != b.serviceStartMinute ||
            a.completionMinute != b.completionMinute) {
            ++intervalChanges;
        }
    }
    return {assignmentChanges, intervalChanges};
}

template<typename Value>
Value median(std::vector<Value> values) {
    std::sort(values.begin(), values.end());
    return values[values.size() / 2U];
}

void printStrategy(
    const std::string& name,
    AssignmentStrategy strategy,
    const std::vector<WorkerMeasurement>& repetitions
) {
    std::vector<std::uint64_t> runtimes;
    std::vector<std::uint64_t> peakMemory;
    std::vector<std::size_t> estimatedBytes;
    for (const WorkerMeasurement& run : repetitions) {
        runtimes.push_back(run.runtimeMicroseconds);
        peakMemory.push_back(run.peakMemoryKiB);
        estimatedBytes.push_back(run.estimatedResultBytes);
    }
    const WorkerMeasurement& result = repetitions.front();
    double averageUtilization = 0.0;
    for (const LaneMeasurement& lane : result.lanes) {
        averageUtilization += lane.utilizationPercent;
    }
    if (!result.lanes.empty()) {
        averageUtilization /= static_cast<double>(result.lanes.size());
    }
    const auto [minimumRuntime, maximumRuntime] =
        std::minmax_element(runtimes.begin(), runtimes.end());
    const auto [minimumMemory, maximumMemory] =
        std::minmax_element(peakMemory.begin(), peakMemory.end());
    std::cout << "  " << std::left << std::setw(20) << toString(strategy)
              << " runtime median/range(us)=" << median(runtimes)
              << '/' << *minimumRuntime << '-' << *maximumRuntime
#if defined(_WIN32)
              << " peak working set median/range(KiB)=" << median(peakMemory)
#else
              << " peak RSS median/range(KiB)=" << median(peakMemory)
#endif
              << '/' << *minimumMemory << '-' << *maximumMemory
              << " estimated-result-bytes(median)=" << median(estimatedBytes)
              << " completed=" << result.completedCount
              << " mean/max-wait=" << std::fixed << std::setprecision(2)
              << result.meanWaitMinutes << '/' << result.maximumWaitMinutes
              << " throughput/h=" << result.throughputPerHour
              << " avg-lane-utilization=" << averageUtilization << "%\n";
    (void)name;
}

int runParent(const char* executableName) {
    const std::filesystem::path executable =
        std::filesystem::absolute(executableName);
    const std::vector<BenchmarkDataset> datasets{
        {"small", 100U, 6U, SyntheticArrivalPattern::Steady},
        {"medium", 5'000U, 6U, SyntheticArrivalPattern::Steady},
        {"large", 50'000U, 6U, SyntheticArrivalPattern::Steady},
        {"adversarial-burst", 50'000U, 6U,
         SyntheticArrivalPattern::SynchronizedBurst},
        {"assignment-divergence", 3U, 2U, SyntheticArrivalPattern::Steady}
    };
    const std::array<AssignmentStrategy, 2U> strategies{
        AssignmentStrategy::ShortestWorkload,
        AssignmentStrategy::RoundRobin
    };

    std::cout << "Airport strategy benchmark; deterministic synthetic data; "
                 "3 isolated repetitions per strategy.\n"
#if defined(_WIN32)
                 "Windows peak working set is PeakWorkingSetSize in KiB per "
                 "CreateProcess-isolated worker.\n"
#elif defined(__APPLE__)
                 "macOS peak RSS is ru_maxrss converted from bytes to KiB per "
                 "exec-isolated worker.\n"
#else
                 "POSIX peak RSS is ru_maxrss in KiB, process-lifetime high-water "
                 "from an exec-isolated worker.\n"
#endif
                 "Estimated result storage is reported separately; correctness "
                 "and schedule checks precede timings.\n";
    bool foundStrategyDifference = false;
    for (const BenchmarkDataset& dataset : datasets) {
        const AirportScenario scenario = makeBenchmarkScenario(dataset);
        std::array<WorkerMeasurement, 2U> preflightResults;
        for (std::size_t strategyIndex = 0;
             strategyIndex < strategies.size();
             ++strategyIndex) {
            preflightResults[strategyIndex] = parseWorkerOutput(
                executeWorker(
                    executable,
                    dataset,
                    strategies[strategyIndex],
                    true
                )
            );
            validateWorkerMeasurement(preflightResults[strategyIndex], scenario);
        }
        const auto [preflightAssignmentChanges, ignoredIntervalChanges] =
            countScheduleDifferences(preflightResults[0], preflightResults[1]);
        (void)ignoredIntervalChanges;
        if (dataset.name == "assignment-divergence" &&
            preflightAssignmentChanges == 0U) {
            throw std::runtime_error(
                "The strategy-divergence preflight did not distinguish assignments."
            );
        }
        std::cout << "\nDataset " << dataset.name
                  << " correctness preflight passed for both strategies; "
                     "starting timed repetitions.\n";

        std::array<std::vector<WorkerMeasurement>, 2U> results;
        for (std::size_t strategyIndex = 0;
             strategyIndex < strategies.size();
             ++strategyIndex) {
            auto& repetitions = results[strategyIndex];
            repetitions.reserve(kRepetitions);
            for (std::size_t repetition = 0; repetition < kRepetitions; ++repetition) {
                const WorkerMeasurement measurement = parseWorkerOutput(
                    executeWorker(
                        executable,
                        dataset,
                        strategies[strategyIndex],
                        false
                    )
                );
                validateWorkerMeasurement(measurement, scenario);
                if (!sameSchedule(preflightResults[strategyIndex], measurement)) {
                    throw std::runtime_error(
                        "A timed repetition differed from its validated preflight schedule."
                    );
                }
                if (!repetitions.empty() &&
                    !sameSchedule(repetitions.front(), measurement)) {
                    throw std::runtime_error(
                        "A strategy produced non-deterministic schedules across repetitions."
                    );
                }
                repetitions.push_back(measurement);
            }
        }

        const WorkerMeasurement& shortest = results[0].front();
        const WorkerMeasurement& roundRobin = results[1].front();
        const auto [assignmentChanges, intervalChanges] =
            countScheduleDifferences(shortest, roundRobin);
        if (dataset.name == "assignment-divergence" && assignmentChanges == 0U) {
            throw std::runtime_error(
                "The strategy-divergence workload did not distinguish assignments."
            );
        }
        foundStrategyDifference = foundStrategyDifference || assignmentChanges > 0U;

        std::cout << "\nDataset " << dataset.name
                  << " passengers=" << scenario.passengers.size()
                  << " lanes=" << scenario.configuration.laneCount
                  << " strategy-schedule-differences: assignments="
                  << assignmentChanges << ", service-intervals="
                  << intervalChanges << '\n';
        printStrategy(dataset.name, strategies[0], results[0]);
        printStrategy(dataset.name, strategies[1], results[1]);
        const bool sameAggregates =
            shortest.meanWaitMinutes == roundRobin.meanWaitMinutes &&
            shortest.maximumWaitMinutes == roundRobin.maximumWaitMinutes &&
            shortest.throughputPerHour == roundRobin.throughputPerHour;
        if (sameAggregates && assignmentChanges > 0U) {
            std::cout << "  Aggregate service metrics match despite different "
                         "passenger assignments.\n";
        }
    }
    if (!foundStrategyDifference) {
        throw std::runtime_error(
            "No benchmark workload distinguished the assignment strategies."
        );
    }
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    try {
        if (argc > 1 && std::string_view(argv[1]) == "--worker") {
            return runWorker(argc, argv);
        }
        return runParent(argv[0]);
    } catch (const std::exception& error) {
        std::cerr << "Airport scenario benchmark failed: " << error.what() << '\n';
        return 1;
    }
}
