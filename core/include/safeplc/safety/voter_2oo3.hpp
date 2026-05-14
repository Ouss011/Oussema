// SPDX-License-Identifier: MIT
// 2-out-of-3 voter (redundant input qualification).
// Majority wins. Persistent disagreement (longer than tolerance) raises a
// discrepancy fault. All-channels-low or all-high are always Ok.

#pragma once

#include <cstdint>

namespace safeplc::safety {

class Voter2oo3 {
public:
    enum class FaultMode : std::uint8_t { ok, discrepancy };

    struct Result {
        bool value;
        FaultMode fault;
    };

    explicit Voter2oo3(std::uint32_t tolerance_ticks = 0U) noexcept
        : tolerance_(tolerance_ticks) {}

    [[nodiscard]] Result vote(bool a, bool b, bool c) noexcept {
        const int sum = static_cast<int>(a) + static_cast<int>(b) + static_cast<int>(c);
        if (sum == 0 || sum == 3) {
            discrepancy_ticks_ = 0U;
            return Result{sum == 3, FaultMode::ok};
        }
        ++discrepancy_ticks_;
        const bool majority = (sum >= 2);
        const FaultMode fault =
            (discrepancy_ticks_ > tolerance_) ? FaultMode::discrepancy : FaultMode::ok;
        return Result{majority, fault};
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
