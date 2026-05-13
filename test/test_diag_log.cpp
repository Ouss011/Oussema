// SPDX-License-Identifier: MIT
#include "safeplc/diag/diag_log.hpp"

#include <gtest/gtest.h>

using safeplc::diag::DiagEntry;
using safeplc::diag::DiagLog;
using safeplc::diag::DiagSeverity;

namespace {
DiagEntry make_entry(std::uint32_t t, DiagSeverity s, std::uint16_t code) {
    return DiagEntry{t, s, code};
}
}  // namespace

TEST(DiagLog, StartsEmpty) {
    DiagLog<4> log;
    EXPECT_TRUE(log.empty());
    EXPECT_FALSE(log.full());
    EXPECT_EQ(log.size(), 0U);
    EXPECT_EQ(DiagLog<4>::capacity(), 4U);
}

TEST(DiagLog, PushIncreasesSize) {
    DiagLog<4> log;
    log.push(make_entry(10U, DiagSeverity::Info, 1U));
    EXPECT_EQ(log.size(), 1U);
    EXPECT_FALSE(log.empty());
}

TEST(DiagLog, PopReturnsOldestFirst) {
    DiagLog<4> log;
    log.push(make_entry(10U, DiagSeverity::Info, 1U));
    log.push(make_entry(20U, DiagSeverity::Warning, 2U));
    log.push(make_entry(30U, DiagSeverity::Error, 3U));
    auto a = log.pop();
    auto b = log.pop();
    auto c = log.pop();
    ASSERT_TRUE(a && b && c);
    EXPECT_EQ(a->code, 1U);
    EXPECT_EQ(b->code, 2U);
    EXPECT_EQ(c->code, 3U);
    EXPECT_TRUE(log.empty());
}

TEST(DiagLog, PopOnEmptyReturnsNullopt) {
    DiagLog<4> log;
    EXPECT_FALSE(log.pop().has_value());
}

TEST(DiagLog, FullStateDetected) {
    DiagLog<2> log;
    log.push(make_entry(1U, DiagSeverity::Info, 1U));
    log.push(make_entry(2U, DiagSeverity::Info, 2U));
    EXPECT_TRUE(log.full());
}

TEST(DiagLog, OverflowOverwritesOldest) {
    DiagLog<2> log;
    log.push(make_entry(1U, DiagSeverity::Info, 1U));
    log.push(make_entry(2U, DiagSeverity::Info, 2U));
    log.push(make_entry(3U, DiagSeverity::Critical, 3U));  // evicts 1U
    auto a = log.pop();
    auto b = log.pop();
    ASSERT_TRUE(a && b);
    EXPECT_EQ(a->code, 2U);
    EXPECT_EQ(b->code, 3U);
}

TEST(DiagLog, ClearEmptiesBuffer) {
    DiagLog<4> log;
    log.push(make_entry(1U, DiagSeverity::Info, 1U));
    log.push(make_entry(2U, DiagSeverity::Info, 2U));
    log.clear();
    EXPECT_TRUE(log.empty());
    EXPECT_FALSE(log.pop().has_value());
}
