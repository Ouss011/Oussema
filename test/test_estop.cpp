// SPDX-License-Identifier: MIT
#include "safeplc/safety/estop.hpp"

#include <gtest/gtest.h>

using safeplc::safety::EStop;

// ---- Normally-closed wiring (active-low, default) -------------------------

TEST(EStop_NC, StartsInSafeState) {
    EStop e;
    EXPECT_EQ(e.state(), EStop::State::Safe);
    EXPECT_FALSE(e.triggered());
}

TEST(EStop_NC, RawHighKeepsSafe) {
    EStop e;
    EXPECT_FALSE(e.update(true));  // NC: 1 = safe
    EXPECT_EQ(e.state(), EStop::State::Safe);
}

TEST(EStop_NC, RawLowTriggers) {
    EStop e;
    EXPECT_TRUE(e.update(false));  // NC: 0 = pressed
    EXPECT_EQ(e.state(), EStop::State::Triggered);
}

TEST(EStop_NC, RemainsLatchedWhileButtonStillPressed) {
    EStop e;
    e.update(false);                  // press
    EXPECT_FALSE(e.reset());          // reset must fail
    EXPECT_TRUE(e.triggered());
}

TEST(EStop_NC, ResetSucceedsAfterRelease) {
    EStop e;
    e.update(false);                  // press
    e.update(true);                   // release
    EXPECT_TRUE(e.reset());
    EXPECT_EQ(e.state(), EStop::State::Safe);
}

TEST(EStop_NC, RetriggerAfterResetIsPossible) {
    EStop e;
    e.update(false);
    e.update(true);
    e.reset();
    EXPECT_TRUE(e.update(false));
    EXPECT_TRUE(e.triggered());
}

TEST(EStop_NC, ResetFromSafeStateIsNoOp) {
    EStop e;
    EXPECT_FALSE(e.reset());
    EXPECT_EQ(e.state(), EStop::State::Safe);
}

// ---- Active-high wiring ---------------------------------------------------

TEST(EStop_AH, ActiveHighRawHighTriggers) {
    EStop e(false);
    EXPECT_TRUE(e.update(true));
    EXPECT_TRUE(e.triggered());
}

TEST(EStop_AH, ActiveHighRawLowKeepsSafe) {
    EStop e(false);
    EXPECT_FALSE(e.update(false));
    EXPECT_EQ(e.state(), EStop::State::Safe);
}

TEST(EStop_AH, ResetFlowMirrorsNC) {
    EStop e(false);
    e.update(true);
    EXPECT_FALSE(e.reset());
    e.update(false);
    EXPECT_TRUE(e.reset());
}
