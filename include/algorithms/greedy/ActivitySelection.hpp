#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace project31::algorithms {

struct Activity {
    std::string id;
    std::int64_t start;
    std::int64_t finish;
};

[[nodiscard]] std::vector<Activity> selectActivities(std::vector<Activity> activities);

} // namespace project31::algorithms
