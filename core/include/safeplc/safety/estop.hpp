// SPDX-License-Identifier: MIT
// Emergency Stop with latching behaviour (EN ISO 13850).
//
// Once triggered, output stays ACTIVE until an explicit reset() is called AND
// the input has returned to inactive state. This prevents accidental restart
// while the E-Stop button is still pressed.
//
// Inputs are normally-closed logic: the channel value is TRUE when SAFE and
// FALSE when the button is pressed. update() takes the *raw* electrical state
// so the caller documents this once and the class internalises the semantics.

#pragma once

#include <cstdint>

namespace safeplc::safety {

class EStop {
public:
    enum class State : std::uint8_t {
        safe,        ///< Button released, no fault, output inactive.
        triggered,   ///< Button pressed OR latched after release pending reset.
    };

    /// @param input_active_low  true if the underlying signal uses NC contact
    ///                          (1 = safe, 0 = pressed). Default = true (NC).
    explicit EStop(bool input_active_low = true) noexcept
        : input_active_low_(input_active_low) {}

    /// Feed the raw electrical input level into the E-Stop logic.
    /// Returns the *triggered* status (true = E-Stop active).
    bool update(bool raw_input) noexcept {
        const bool pressed = input_active_low_ ? !raw_input : raw_input;
        if (pressed) {
            state_ = State::triggered;
            input_released_since_trigger_ = false;
        } else if (state_ == State::triggered) {
            input_released_since_trigger_ = true;
        }
        return state_ == State::triggered;
    }

    /// Acknowledge a trigger. Only returns to Safe if the input has been
    /// released at least once since the trigger (anti-defeat).
    /// @return true if the reset was effective.
    bool reset() noexcept {
        if (state_ == State::triggered && input_released_since_trigger_) {
            state_ = State::safe;
            input_released_since_trigger_ = false;
            return true;
        }
        return false;
    }

    [[nodiscard]] State state() const noexcept { return state_; }
    [[nodiscard]] bool triggered() const noexcept { return state_ == State::triggered; }

private:
    bool input_active_low_;
    State state_ = State::safe;
    bool input_released_since_trigger_ = false;
};

}  // namespace safeplc::safety
