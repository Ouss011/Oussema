// SPDX-License-Identifier: MIT
#include "safeplc/safety/two_hand.hpp"

#include <gtest/gtest.h>

#include "fakes/fake_clock.hpp"

using safeplc::safety::TwoHandControl;

class TwoHandTest : public ::testing::Test {
protected:
    FakeClock clock;
};

TEST_F(TwoHandTest, BothReleasedOutputOff) {
    TwoHandControl th(clock);
    EXPECT_FALSE(th.update(false, false));
}

TEST_F(TwoHandTest, OnlyLeftPressedOutputOff) {
    TwoHandControl th(clock);
    EXPECT_FALSE(th.update(true, false));
}

TEST_F(TwoHandTest, OnlyRightPressedOutputOff) {
    TwoHandControl th(clock);
    EXPECT_FALSE(th.update(false, true));
}

TEST_F(TwoHandTest, BothPressedSimultaneouslyOutputOn) {
    TwoHandControl th(clock, 500U);
    EXPECT_TRUE(th.update(true, true));
}

TEST_F(TwoHandTest, BothPressedWithinSyncWindowOutputOn) {
    TwoHandControl th(clock, 500U);
    EXPECT_FALSE(th.update(true, false));
    clock.advance(300U);
    EXPECT_TRUE(th.update(true, true));
}

TEST_F(TwoHandTest, SyncWindowExceededDisarms) {
    TwoHandControl th(clock, 500U);
    th.update(true, false);
    clock.advance(800U);
    EXPECT_FALSE(th.update(true, true));
    EXPECT_FALSE(th.armed());
}

TEST_F(TwoHandTest, DisarmedRequiresFullReleaseToReArm) {
    TwoHandControl th(clock, 100U);
    th.update(true, false);
    clock.advance(200U);
    th.update(true, true);            // disarm
    EXPECT_FALSE(th.update(true, true));  // still disarmed
    EXPECT_FALSE(th.update(false, false));
    EXPECT_TRUE(th.armed());
    EXPECT_TRUE(th.update(true, true));
}

TEST_F(TwoHandTest, ReleasingOneButtonDisablesOutput) {
    TwoHandControl th(clock, 500U);
    EXPECT_TRUE(th.update(true, true));
    EXPECT_FALSE(th.update(true, false));
}
