#include "simulator/AirportScenario.hpp"

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::filesystem::path dataDirectory() {
    return std::filesystem::path("data");
}

void runAndExport(
    const project31::AirportScenario& scenario,
    const std::filesystem::path& outputDirectory,
    const std::string& filePrefix
) {
    const project31::ScenarioComparison comparison =
        project31::AirportScenarioRunner::compareStrategies(scenario);
    const std::filesystem::path basePath = outputDirectory / filePrefix;
    project31::AirportScenarioRunner::exportCsv(
        basePath.string() + ".csv",
        comparison
    );
    project31::AirportScenarioRunner::exportJson(
        basePath.string() + ".json",
        comparison
    );
    std::cout << scenario.name << ": "
              << comparison.shortestWorkload.completedPassengerCount
              << " passengers screened; shortest-workload mean wait "
              << std::fixed << std::setprecision(2)
              << comparison.shortestWorkload.meanWaitMinutes
              << " minutes; round-robin mean wait "
              << comparison.roundRobin.meanWaitMinutes
              << " minutes; recommended strategy "
              << comparison.recommendedStrategy << ".\n"
              << "  CSV: " << basePath.string() << ".csv\n"
              << "  JSON: " << basePath.string() << ".json\n"
              << "  " << comparison.resourceRecommendation << '\n';
}

} // namespace

int main(int argc, char** argv) {
    try {
        const std::filesystem::path outputDirectory =
            argc > 1 ? argv[1] : "scenario-results";
        std::filesystem::create_directories(outputDirectory);

        const auto normal = project31::AirportScenarioRunner::loadCsvFile(
            dataDirectory() / "normal.csv",
            "Normal operation",
            {2U, 2U, {}}
        );
        const auto shock = project31::AirportScenarioRunner::loadCsvFile(
            dataDirectory() / "capacity_shock.csv",
            "Capacity and staffing shock",
            {3U, 3U, {{0U, 3, 12}}}
        );
        const auto stress = project31::AirportScenarioRunner::makeSyntheticScenario(
            "Large-data stress",
            50'000U,
            {6U, 6U, {}}
        );

        runAndExport(normal, outputDirectory, "normal");
        runAndExport(shock, outputDirectory, "capacity_shock");
        runAndExport(stress, outputDirectory, "large_stress");
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Airport scenario run failed: " << error.what() << '\n';
        return 1;
    }
}
