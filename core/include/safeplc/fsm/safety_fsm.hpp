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

enum class SafetyState : std::uint8_t { init, pre_run, run, fault };

enum class SafetyEvent : std::uint8_t {
    init_ok,
    init_fail,
    run_request,
    stop,
    fault,
    reset,
};

class SafetyFsm {
public:
    SafetyFsm() = default;

    /// Process an external event and return the resulting state.
    SafetyState on_event(SafetyEvent ev) noexcept {
        switch (state_) {
        case SafetyState::init:
            if (ev == SafetyEvent::init_ok)        { state_ = SafetyState::pre_run; }
            else if (ev == SafetyEvent::init_fail) { state_ = SafetyState::fault; }
            else if (ev == SafetyEvent::fault)     { state_ = SafetyState::fault; }
            break;
        case SafetyState::pre_run:
            if (ev == SafetyEvent::run_request) { state_ = SafetyState::run; }
            else if (ev == SafetyEvent::fault)  { state_ = SafetyState::fault; }
            break;
        case SafetyState::run:
            if (ev == SafetyEvent::stop)        { state_ = SafetyState::pre_run; }
            else if (ev == SafetyEvent::fault)  { state_ = SafetyState::fault; }
            break;
        case SafetyState::fault:
            if (ev == SafetyEvent::reset) { state_ = SafetyState::init; }
            break;
        }
        return state_;
    }

    [[nodiscard]] SafetyState state() const noexcept { return state_; }

    [[nodiscard]] bool is_safe_to_actuate() const noexcept {
        return state_ == SafetyState::run;
    }

private:
    SafetyState state_ = SafetyState::init;
};

}  // namespace safeplc::fsm
