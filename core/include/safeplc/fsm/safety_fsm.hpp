// SPDX-License-Identifier: MIT
// Top-level safety state machine.
//
//        +---------+   init_ok    +-----------+   run_request   +---------+
//        |  Init   |------------->| PreRun    |---------------->|   Run   |
//        +---------+              +-----------+                 +---------+
//             |                          |                            |
//             | init_fail                | fault                      | fault
//             v                          v                            v
//        +---------+   reset (after release of E-Stop and operator ack) +
//        |  Fault  |<---------------------------------------------------+
//        +---------+
//
// Default state on construction is Init. Any fault forces the FSM to Fault.

#pragma once

#include <cstdint>

namespace safeplc::fsm {

enum class SafetyState : std::uint8_t { Init, PreRun, Run, Fault };

enum class SafetyEvent : std::uint8_t {
    InitOk,
    InitFail,
    RunRequest,
    Stop,
    Fault,
    Reset,
};

class SafetyFsm {
public:
    SafetyFsm() = default;

    /// Process an external event and return the resulting state.
    SafetyState on_event(SafetyEvent ev) noexcept {
        switch (state_) {
        case SafetyState::Init:
            if (ev == SafetyEvent::InitOk)        { state_ = SafetyState::PreRun; }
            else if (ev == SafetyEvent::InitFail) { state_ = SafetyState::Fault; }
            else if (ev == SafetyEvent::Fault)    { state_ = SafetyState::Fault; }
            break;
        case SafetyState::PreRun:
            if (ev == SafetyEvent::RunRequest) { state_ = SafetyState::Run; }
            else if (ev == SafetyEvent::Fault) { state_ = SafetyState::Fault; }
            break;
        case SafetyState::Run:
            if (ev == SafetyEvent::Stop)       { state_ = SafetyState::PreRun; }
            else if (ev == SafetyEvent::Fault) { state_ = SafetyState::Fault; }
            break;
        case SafetyState::Fault:
            if (ev == SafetyEvent::Reset) { state_ = SafetyState::Init; }
            break;
        }
        return state_;
    }

    [[nodiscard]] SafetyState state() const noexcept { return state_; }

    [[nodiscard]] bool is_safe_to_actuate() const noexcept {
        return state_ == SafetyState::Run;
    }

private:
    SafetyState state_ = SafetyState::Init;
};

}  // namespace safeplc::fsm
