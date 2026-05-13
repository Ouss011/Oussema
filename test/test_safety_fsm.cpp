// SPDX-License-Identifier: MIT
#include "safeplc/fsm/safety_fsm.hpp"

#include <gtest/gtest.h>

using safeplc::fsm::SafetyEvent;
using safeplc::fsm::SafetyFsm;
using safeplc::fsm::SafetyState;

TEST(SafetyFsm, InitialStateIsInit) {
    SafetyFsm fsm;
    EXPECT_EQ(fsm.state(), SafetyState::Init);
    EXPECT_FALSE(fsm.is_safe_to_actuate());
}

TEST(SafetyFsm, InitToPreRunOnInitOk) {
    SafetyFsm fsm;
    EXPECT_EQ(fsm.on_event(SafetyEvent::InitOk), SafetyState::PreRun);
}

TEST(SafetyFsm, InitToFaultOnInitFail) {
    SafetyFsm fsm;
    EXPECT_EQ(fsm.on_event(SafetyEvent::InitFail), SafetyState::Fault);
}

TEST(SafetyFsm, InitToFaultOnFaultEvent) {
    SafetyFsm fsm;
    EXPECT_EQ(fsm.on_event(SafetyEvent::Fault), SafetyState::Fault);
}

TEST(SafetyFsm, InitIgnoresIrrelevantEvents) {
    SafetyFsm fsm;
    EXPECT_EQ(fsm.on_event(SafetyEvent::RunRequest), SafetyState::Init);
    EXPECT_EQ(fsm.on_event(SafetyEvent::Stop), SafetyState::Init);
    EXPECT_EQ(fsm.on_event(SafetyEvent::Reset), SafetyState::Init);
}

TEST(SafetyFsm, PreRunToRunOnRunRequest) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::InitOk);
    EXPECT_EQ(fsm.on_event(SafetyEvent::RunRequest), SafetyState::Run);
    EXPECT_TRUE(fsm.is_safe_to_actuate());
}

TEST(SafetyFsm, PreRunToFaultOnFault) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::InitOk);
    EXPECT_EQ(fsm.on_event(SafetyEvent::Fault), SafetyState::Fault);
}

TEST(SafetyFsm, PreRunIgnoresIrrelevantEvents) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::InitOk);
    EXPECT_EQ(fsm.on_event(SafetyEvent::InitOk), SafetyState::PreRun);
    EXPECT_EQ(fsm.on_event(SafetyEvent::Stop), SafetyState::PreRun);
}

TEST(SafetyFsm, RunToPreRunOnStop) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::InitOk);
    fsm.on_event(SafetyEvent::RunRequest);
    EXPECT_EQ(fsm.on_event(SafetyEvent::Stop), SafetyState::PreRun);
}

TEST(SafetyFsm, RunToFaultOnFault) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::InitOk);
    fsm.on_event(SafetyEvent::RunRequest);
    EXPECT_EQ(fsm.on_event(SafetyEvent::Fault), SafetyState::Fault);
    EXPECT_FALSE(fsm.is_safe_to_actuate());
}

TEST(SafetyFsm, RunIgnoresIrrelevantEvents) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::InitOk);
    fsm.on_event(SafetyEvent::RunRequest);
    EXPECT_EQ(fsm.on_event(SafetyEvent::Reset), SafetyState::Run);
    EXPECT_EQ(fsm.on_event(SafetyEvent::InitOk), SafetyState::Run);
}

TEST(SafetyFsm, FaultToInitOnReset) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::InitFail);
    EXPECT_EQ(fsm.on_event(SafetyEvent::Reset), SafetyState::Init);
}

TEST(SafetyFsm, FaultIgnoresNonResetEvents) {
    SafetyFsm fsm;
    fsm.on_event(SafetyEvent::InitFail);
    EXPECT_EQ(fsm.on_event(SafetyEvent::RunRequest), SafetyState::Fault);
    EXPECT_EQ(fsm.on_event(SafetyEvent::InitOk), SafetyState::Fault);
    EXPECT_EQ(fsm.on_event(SafetyEvent::Fault), SafetyState::Fault);
}
