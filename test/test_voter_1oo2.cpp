// SPDX-License-Identifier: MIT
#include "safeplc/safety/voter_1oo2.hpp"

#include <gtest/gtest.h>

using safeplc::safety::Voter1oo2;

TEST(Voter1oo2, BothLowReturnsLowOk) {
    Voter1oo2 v;
    const auto r = v.vote(false, false);
    EXPECT_FALSE(r.value);
    EXPECT_EQ(r.fault, Voter1oo2::FaultMode::ok);
}

TEST(Voter1oo2, BothHighReturnsHighOk) {
    Voter1oo2 v;
    const auto r = v.vote(true, true);
    EXPECT_TRUE(r.value);
    EXPECT_EQ(r.fault, Voter1oo2::FaultMode::ok);
}

TEST(Voter1oo2, DiscrepancyImmediateWhenToleranceZero) {
    Voter1oo2 v(0U);
    const auto r = v.vote(true, false);
    EXPECT_TRUE(r.value);  // OR
    EXPECT_EQ(r.fault, Voter1oo2::FaultMode::discrepancy);
}

TEST(Voter1oo2, DiscrepancyTransientWithinTolerance) {
    Voter1oo2 v(3U);
    for (int i = 0; i < 3; ++i) {
        const auto r = v.vote(false, true);
        EXPECT_TRUE(r.value);
        EXPECT_EQ(r.fault, Voter1oo2::FaultMode::ok);
    }
}

TEST(Voter1oo2, DiscrepancyPersistsBeyondTolerance) {
    Voter1oo2 v(2U);
    (void)v.vote(true, false);
    (void)v.vote(true, false);
    const auto r = v.vote(true, false);
    EXPECT_EQ(r.fault, Voter1oo2::FaultMode::discrepancy);
    EXPECT_EQ(v.discrepancy_ticks(), 3U);
}

TEST(Voter1oo2, AgreementClearsDiscrepancyCounter) {
    Voter1oo2 v(5U);
    (void)v.vote(true, false);
    (void)v.vote(true, false);
    EXPECT_EQ(v.discrepancy_ticks(), 2U);
    (void)v.vote(true, true);
    EXPECT_EQ(v.discrepancy_ticks(), 0U);
}

TEST(Voter1oo2, ResetClearsCounter) {
    Voter1oo2 v(0U);
    (void)v.vote(true, false);
    EXPECT_NE(v.discrepancy_ticks(), 0U);
    v.reset();
    EXPECT_EQ(v.discrepancy_ticks(), 0U);
}
