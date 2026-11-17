#include "simulator/AirportScenario.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

namespace project31 {

TEST(AirportSchedulingPriorityUnitTests, RiskCategoriesHaveExplicitPriorityRanks) {
    EXPECT_EQ(effectivePriorityRank(PassengerRiskCategory::High, 0, 10), 0);
    EXPECT_EQ(effectivePriorityRank(PassengerRiskCategory::Elevated, 0, 10), 1);
    EXPECT_EQ(effectivePriorityRank(PassengerRiskCategory::Standard, 0, 10), 2);
}

TEST(AirportSchedulingPriorityUnitTests, AgingPromotesOneRiskLevelPerInterval) {
    EXPECT_EQ(effectivePriorityRank(PassengerRiskCategory::Standard, 9, 10), 2);
    EXPECT_EQ(effectivePriorityRank(PassengerRiskCategory::Standard, 10, 10), 1);
    EXPECT_EQ(effectivePriorityRank(PassengerRiskCategory::Standard, 20, 10), 0);
    EXPECT_EQ(effectivePriorityRank(PassengerRiskCategory::Elevated, 10, 10), 0);
}

TEST(AirportSchedulingPriorityUnitTests, RejectsInvalidWaitAndAgingIntervals) {
    EXPECT_THROW(
        effectivePriorityRank(PassengerRiskCategory::Standard, -1, 10),
        std::invalid_argument
    );
    EXPECT_THROW(
        effectivePriorityRank(PassengerRiskCategory::Standard, 0, 0),
        std::invalid_argument
    );
}

} // namespace project31
