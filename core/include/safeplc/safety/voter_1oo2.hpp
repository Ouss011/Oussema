// SPDX-License-Identifier: MIT
// 1-out-of-2 voter (fail-safe).
// Returns the OR of the two channels: any active channel triggers output.
// Discrepancy is reported if channels disagree longer than tolerance ticks.

#pragma once

#include <cstdint>

namespace safeplc::safety {

class Voter1oo2 {
public:
    enum class FaultMode : std::uint8_t { ok, discrepancy };

    struct Result {
        bool value;
        FaultMode fault;
    };

    explicit Voter1oo2(std::uint32_t tolerance_ticks = 0U) noexcept
        : tolerance_(tolerance_ticks) {}

    [[nodiscard]] Result vote(bool a, bool b) noexcept {
        const bool output = a || b;
        if (a == b) {
            discrepancy_ticks_ = 0U;
            return Result{output, FaultMode::ok};
        }
        ++discrepancy_ticks_;
        const FaultMode fault =
            (discrepancy_ticks_ > tolerance_) ? FaultMode::discrepancy : FaultMode::ok;
        return Result{output, fault};
    }

    void reset() noexcept { discrepancy_ticks_ = 0U; }

    [[nodiscard]] std::uint32_t discrepancy_ticks() const noexcept {
        return discrepancy_ticks_;
    }

private:
    std::uint32_t tolerance_;
    std::uint32_t discrepancy_ticks_ = 0U;
};

}  // namespace safeplc::safety
