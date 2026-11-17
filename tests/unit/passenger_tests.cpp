#include "core/Passenger.hpp"

#include <gtest/gtest.h>

namespace project31 {

TEST(PassengerTests, ValidPassengerHasNoValidationErrors) {
    const Passenger passenger("P-1", "Ada Lovelace", 4);

    EXPECT_TRUE(passenger.validationErrors().empty());
    EXPECT_EQ(passenger.status(), PassengerStatus::Registered);
}

TEST(PassengerTests, RejectsBlankFieldsAndNonpositiveScreeningDuration) {
    const Passenger passenger(" \t", "", 0);

    EXPECT_EQ(passenger.validationErrors().size(), 3U);
}

TEST(PassengerTests, EnforcesPassengerStatusTransitions) {
    Passenger passenger("P-1", "Ada Lovelace", 4);

    EXPECT_THROW(passenger.markScreened(), std::logic_error);
    passenger.markQueued();
    EXPECT_EQ(passenger.status(), PassengerStatus::Queued);
    passenger.markScreened();
    EXPECT_EQ(passenger.status(), PassengerStatus::Screened);
}

} // namespace project31
