#include "algorithms/greedy/IntervalPartitioning.hpp"

#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace project31::algorithms {
namespace {

struct IndexedInterval {
    ScreeningInterval interval;
    std::size_t originalIndex;
};

bool before(const IndexedInterval& left, const IndexedInterval& right) {
    if (left.interval.start != right.interval.start) {
        return left.interval.start < right.interval.start;
    }
    if (left.interval.finish != right.interval.finish) {
        return left.interval.finish < right.interval.finish;
    }
    return left.originalIndex < right.originalIndex;
}

void mergeSort(std::vector<IndexedInterval>& values,
               std::vector<IndexedInterval>& buffer,
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

struct ActiveResource {
    std::int64_t finish;
    std::size_t resource;
};

bool heapBefore(const ActiveResource& left, const ActiveResource& right) {
    return left.finish < right.finish ||
        (left.finish == right.finish && left.resource < right.resource);
}

void heapPush(std::vector<ActiveResource>& heap, ActiveResource value) {
    heap.push_back(value);
    std::size_t child = heap.size() - 1;
    while (child > 0) {
        const std::size_t parent = (child - 1) / 2;
        if (!heapBefore(heap[child], heap[parent])) {
            break;
        }
        std::swap(heap[child], heap[parent]);
        child = parent;
    }
}

ActiveResource heapPop(std::vector<ActiveResource>& heap) {
    const ActiveResource result = heap.front();
    heap.front() = heap.back();
    heap.pop_back();
    std::size_t parent = 0;
    while (parent < heap.size()) {
        const std::size_t left = parent * 2 + 1;
        if (left >= heap.size()) {
            break;
        }
        const std::size_t right = left + 1;
        std::size_t child = left;
        if (right < heap.size() && heapBefore(heap[right], heap[left])) {
            child = right;
        }
        if (!heapBefore(heap[child], heap[parent])) {
            break;
        }
        std::swap(heap[parent], heap[child]);
        parent = child;
    }
    return result;
}

} // namespace

IntervalPartition partitionIntervals(std::vector<ScreeningInterval> intervals) {
    std::unordered_set<std::string> ids;
    std::vector<IndexedInterval> ordered;
    ordered.reserve(intervals.size());
    for (std::size_t index = 0; index < intervals.size(); ++index) {
        const ScreeningInterval& interval = intervals[index];
        if (interval.id.find_first_not_of(" \t\n\r\f\v") == std::string::npos ||
            interval.start > interval.finish) {
            throw std::invalid_argument("Intervals require an ID and start <= finish.");
        }
        if (!ids.insert(interval.id).second) {
            throw std::invalid_argument("Interval IDs must be unique.");
        }
        ordered.push_back({interval, index});
    }
    if (ordered.size() > 1) {
        std::vector<IndexedInterval> buffer(ordered.size());
        mergeSort(ordered, buffer, 0, ordered.size());
    }

    std::vector<ActiveResource> active;
    std::vector<IntervalAssignment> assignments;
    assignments.reserve(ordered.size());
    std::size_t resourceCount = 0;
    for (const IndexedInterval& item : ordered) {
        std::size_t resource{};
        if (!active.empty() && active.front().finish <= item.interval.start) {
            resource = heapPop(active).resource;
        } else {
            resource = resourceCount++;
        }
        assignments.push_back({item.interval.id, resource});
        heapPush(active, {item.interval.finish, resource});
    }
    return {resourceCount, std::move(assignments)};
}

} // namespace project31::algorithms
