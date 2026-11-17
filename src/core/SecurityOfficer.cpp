#include "core/SecurityOfficer.hpp"

#include <stdexcept>
#include <utility>

namespace project31 {
namespace {

bool isBlank(const std::string& value) {
    return value.find_first_not_of(" \t\n\r\f\v") == std::string::npos;
}

} // namespace

SecurityOfficer::SecurityOfficer(std::string id, std::string name)
    : id_(std::move(id)), name_(std::move(name)) {
    if (isBlank(id_) || isBlank(name_)) {
        throw std::invalid_argument("Security officer ID and name must not be empty.");
    }
}

const std::string& SecurityOfficer::id() const noexcept {
    return id_;
}

const std::string& SecurityOfficer::name() const noexcept {
    return name_;
}

} // namespace project31
