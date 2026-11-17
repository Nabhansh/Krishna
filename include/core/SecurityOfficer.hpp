#pragma once

#include <string>

namespace project31 {

class SecurityOfficer {
public:
    SecurityOfficer(std::string id, std::string name);

    [[nodiscard]] const std::string& id() const noexcept;
    [[nodiscard]] const std::string& name() const noexcept;

private:
    std::string id_;
    std::string name_;
};

} // namespace project31
