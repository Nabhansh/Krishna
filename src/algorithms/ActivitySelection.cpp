#include "algorithms/greedy/ActivitySelection.hpp"

#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

bool before(const Activity& left, const Activity& right) {
    if (left.finish != right.finish) {
        return left.finish < right.finish;
    }
    if (left.start != right.start) {
        return left.start < right.start;
    }
    return left.id < right.id;
}

void mergeSort(std::vector<Activity>& values, std::vector<Activity>& buffer,
               std::size_t begin, std::size_t end) {
    if (end - begin < 2) {
        return;
    }
    const std::size_t middle = begin + (end - begin) / 2;
    mergeSort(values, buffer, begin, middle);
    mergeSort(values, buffer, middle, end);
    std::size_t left = begin;
    std::size_t right = middle;
    std::size_t out = begin;
    while (left < middle && right < end) {
        buffer[out++] = before(values[right], values[left])
            ? values[right++] : values[left++];
    }
    while (left < middle) {
        buffer[out++] = values[left++];
    }
    while (right < end) {
        buffer[out++] = values[right++];
    }
    for (std::size_t index = begin; index < end; ++index) {
        values[index] = std::move(buffer[index]);
    }
}

} // namespace

std::vector<Activity> selectActivities(std::vector<Activity> activities) {
    std::unordered_set<std::string> ids;
    for (const Activity& activity : activities) {
        if (activity.id.find_first_not_of(" \t\n\r\f\v") == std::string::npos ||
            activity.start > activity.finish) {
            throw std::invalid_argument("Activities require an ID and start <= finish.");
        }
        if (!ids.insert(activity.id).second) {
            throw std::invalid_argument("Activity IDs must be unique.");
        }
    }
    if (activities.size() < 2) {
        return activities;
    }
    std::vector<Activity> buffer(activities.size());
    mergeSort(activities, buffer, 0, activities.size());

    std::vector<Activity> selected;
    selected.reserve(activities.size());
    selected.push_back(activities.front());
    std::int64_t finish = activities.front().finish;
    for (std::size_t index = 1; index < activities.size(); ++index) {
        if (activities[index].start >= finish) {
            selected.push_back(activities[index]);
            finish = activities[index].finish;
        }
    }
    return selected;
}

} // namespace project31::algorithms
