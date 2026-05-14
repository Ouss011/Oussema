// SPDX-License-Identifier: MIT
#include "safeplc/safety/voter_2oo3.hpp"

#include <gtest/gtest.h>

using safeplc::safety::Voter2oo3;

TEST(Voter2oo3, AllLowReturnsLowOk) {
    Voter2oo3 v;
    const auto r = v.vote(false, false, false);
    EXPECT_FALSE(r.value);
    EXPECT_EQ(r.fault, Voter2oo3::FaultMode::ok);
}

TEST(Voter2oo3, AllHighReturnsHighOk) {
    Voter2oo3 v;
    const auto r = v.vote(true, true, true);
    EXPECT_TRUE(r.value);
    EXPECT_EQ(r.fault, Voter2oo3::FaultMode::ok);
}

TEST(Voter2oo3, TwoOfThreeMajorityReturnsHigh) {
    Voter2oo3 v(10U);  // generous tolerance, fault not expected
    EXPECT_TRUE(v.vote(true, true, false).value);
    EXPECT_TRUE(v.vote(true, false, true).value);
    EXPECT_TRUE(v.vote(false, true, true).value);
}

TEST(Voter2oo3, OneOfThreeMinorityReturnsLow) {
    Voter2oo3 v(10U);
    EXPECT_FALSE(v.vote(true, false, false).value);
    EXPECT_FALSE(v.vote(false, true, false).value);
    EXPECT_FALSE(v.vote(false, false, true).value);
}

TEST(Voter2oo3, ImmediateDiscrepancyWhenToleranceZero) {
    Voter2oo3 v(0U);
    const auto r = v.vote(true, true, false);
    EXPECT_TRUE(r.value);
    EXPECT_EQ(r.fault, Voter2oo3::FaultMode::discrepancy);
}

TEST(Voter2oo3, DiscrepancyTransientWithinTolerance) {
    Voter2oo3 v(3U);
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(v.vote(true, true, false).fault, Voter2oo3::FaultMode::ok);
    }
    EXPECT_EQ(v.vote(true, true, false).fault, Voter2oo3::FaultMode::discrepancy);
}

TEST(Voter2oo3, UnanimityClearsDiscrepancyCounter) {
    Voter2oo3 v(5U);
    (void)v.vote(true, true, false);
    (void)v.vote(true, true, false);
    EXPECT_EQ(v.discrepancy_ticks(), 2U);
    (void)v.vote(true, true, true);
    EXPECT_EQ(v.discrepancy_ticks(), 0U);
}

TEST(Voter2oo3, ResetClearsCounter) {
    Voter2oo3 v(0U);
    (void)v.vote(true, false, true);
    v.reset();
    EXPECT_EQ(v.discrepancy_ticks(), 0U);
}
