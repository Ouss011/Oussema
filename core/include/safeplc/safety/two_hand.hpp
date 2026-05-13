// SPDX-License-Identifier: MIT
// Two-Hand Control (EN ISO 13851 type IIIC).
//
// Both buttons must be pressed within a synchronisation window (default 500 ms)
// AND remain pressed simultaneously for the output to be ENABLED. Releasing
// either button immediately disables the output; both must be released before
// re-arming.

#pragma once

#include <cstdint>

#include "safeplc/ports/i_clock.hpp"

namespace safeplc::safety {

class TwoHandControl {
public:
    TwoHandControl(safeplc::ports::IClock& clock,
                   std::uint32_t sync_window_ms = 500U) noexcept
        : clock_(clock), sync_window_ms_(sync_window_ms) {}

    /// @param left   raw active-high state of left button
    /// @param right  raw active-high state of right button
    /// @return       true if output is ENABLED
    bool update(bool left, bool right) noexcept {
        const std::uint32_t now = clock_.millis();

        // Both released → reset arming state.
        if (!left && !right) {
            armed_ = true;
            left_press_ms_ = 0U;
            right_press_ms_ = 0U;
            return false;
        }

        // Only one pressed → record press time, output OFF.
        if (left && !right) {
            if (left_press_ms_ == 0U) {
                left_press_ms_ = now == 0U ? 1U : now;
            }
            return false;
        }
        if (!left && right) {
            if (right_press_ms_ == 0U) {
                right_press_ms_ = now == 0U ? 1U : now;
            }
            return false;
        }

        // Both pressed — check synchronisation.
        if (!armed_) {
            return false;  // must release both to re-arm
        }
        if (left_press_ms_ == 0U) {
            left_press_ms_ = now == 0U ? 1U : now;
        }
        if (right_press_ms_ == 0U) {
            right_press_ms_ = now == 0U ? 1U : now;
        }
        const std::uint32_t delta =
            (left_press_ms_ > right_press_ms_) ? (left_press_ms_ - right_press_ms_)
                                               : (right_press_ms_ - left_press_ms_);
        if (delta > sync_window_ms_) {
            armed_ = false;  // synchronisation lost → require full release
            return false;
        }
        return true;
    }

    [[nodiscard]] bool armed() const noexcept { return armed_; }

private:
    safeplc::ports::IClock& clock_;
    std::uint32_t sync_window_ms_;
    bool armed_ = true;
    std::uint32_t left_press_ms_ = 0U;
    std::uint32_t right_press_ms_ = 0U;
};

}  // namespace safeplc::safety
