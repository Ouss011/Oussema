// SPDX-License-Identifier: MIT
#include "safeplc/fsm/safety_fsm.hpp"

#include <gtest/gtest.h>

using safeplc::fsm::SafetyEvent;
using safeplc::fsm::SafetyFsm;
using safeplc::fsm::SafetyState;

TEST(SafetyFsm, InitialStateIsInit) {
    SafetyFsm fsm;
    EXPECT_EQ(fsm.state(), SafetyState::init);
    EXPECT_FALSE(fsm.is_safe_to_actuate());
}

TEST(SafetyFsm, InitToPreRunOnInitOk) {
    SafetyFsm fsm;
    EXPECT_EQ(fsm.on_event(SafetyEvent::init_ok), SafetyState::pre_run);
}

TEST(SafetyFsm, InitToFaultOnInitFail) {
    SafetyFsm fsm;
    EXPECT_EQ(fsm.on_event(SafetyEvent::init_fail), SafetyState::fault);
}

TEST(SafetyFsm, InitToFaultOnFaultEvent) {
    SafetyFsm fsm;
    EXPECT_EQ(fsm.on_event(SafetyEvent::fault), SafetyState::fault);
}

TEST(SafetyFsm, InitIgnoresIrrelevantEvents) {
    SafetyFsm fsm;
    EXPECT_EQ(fsm.on_event(SafetyEvent::run_request), SafetyState::init);
    EXPECT_EQ(fsm.on_event(SafetyEvent::stop), SafetyState::init);
    EXPECT_EQ(fsm.on_event(SafetyEvent::reset), SafetyState::init);
}

TEST(SafetyFsm, PreRunToRunOnRunRequest) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::init_ok);
    EXPECT_EQ(fsm.on_event(SafetyEvent::run_request), SafetyState::run);
    EXPECT_TRUE(fsm.is_safe_to_actuate());
}

TEST(SafetyFsm, PreRunToFaultOnFault) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::init_ok);
    EXPECT_EQ(fsm.on_event(SafetyEvent::fault), SafetyState::fault);
}

TEST(SafetyFsm, PreRunIgnoresIrrelevantEvents) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::init_ok);
    EXPECT_EQ(fsm.on_event(SafetyEvent::init_ok), SafetyState::pre_run);
    EXPECT_EQ(fsm.on_event(SafetyEvent::stop), SafetyState::pre_run);
}

TEST(SafetyFsm, RunToPreRunOnStop) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::init_ok);
    fsm.on_event(SafetyEvent::run_request);
    EXPECT_EQ(fsm.on_event(SafetyEvent::stop), SafetyState::pre_run);
}

TEST(SafetyFsm, RunToFaultOnFault) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::init_ok);
    fsm.on_event(SafetyEvent::run_request);
    EXPECT_EQ(fsm.on_event(SafetyEvent::fault), SafetyState::fault);
    EXPECT_FALSE(fsm.is_safe_to_actuate());
}

TEST(SafetyFsm, RunIgnoresIrrelevantEvents) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::init_ok);
    fsm.on_event(SafetyEvent::run_request);
    EXPECT_EQ(fsm.on_event(SafetyEvent::reset), SafetyState::run);
    EXPECT_EQ(fsm.on_event(SafetyEvent::init_ok), SafetyState::run);
}

TEST(SafetyFsm, FaultToInitOnReset) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::init_fail);
    EXPECT_EQ(fsm.on_event(SafetyEvent::reset), SafetyState::init);
}

TEST(SafetyFsm, FaultIgnoresNonResetEvents) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::init_fail);
    EXPECT_EQ(fsm.on_event(SafetyEvent::run_request), SafetyState::fault);
    EXPECT_EQ(fsm.on_event(SafetyEvent::init_ok), SafetyState::fault);
    EXPECT_EQ(fsm.on_event(SafetyEvent::fault), SafetyState::fault);
}
