// SPDX-License-Identifier: MIT
// Software watchdog.
//
// The supervised loop must call kick() at least every `timeout_ms`. If kick()
// is not called in time, expired() returns true and stays sticky until
// reset(). Models the IEC 61508 software-watchdog diagnostic.

#pragma once

#include <cstdint>

#include "safeplc/ports/i_clock.hpp"

namespace safeplc::safety {

class Watchdog {
public:
    Watchdog(safeplc::ports::IClock& clock, std::uint32_t timeout_ms) noexcept
        : clock_(clock), timeout_ms_(timeout_ms), last_kick_ms_(clock_.millis()) {}

    /// Refresh the watchdog. Must be called periodically.
    void kick() noexcept { last_kick_ms_ = clock_.millis(); }

    /// True if the watchdog has expired and was not yet reset.
    [[nodiscard]] bool expired() noexcept {
        if (sticky_expired_) {
            return true;
        }
        const std::uint32_t now = clock_.millis();
        if ((now - last_kick_ms_) > timeout_ms_) {
            sticky_expired_ = true;
        }
        return sticky_expired_;
    }

    /// Clear the expired flag. Caller is responsible for handling the fault.
    void reset() noexcept {
        sticky_expired_ = false;
        last_kick_ms_ = clock_.millis();
    }

    [[nodiscard]] std::uint32_t timeout_ms() const noexcept { return timeout_ms_; }

private:
    safeplc::ports::IClock& clock_;
    std::uint32_t timeout_ms_;
    std::uint32_t last_kick_ms_;
    bool sticky_expired_ = false;
};

}  // namespace safeplc::safety
