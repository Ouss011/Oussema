// SPDX-License-Identifier: MIT
// Deterministic clock for unit tests. Manually advanced — never sleeps.

#pragma once

#include <cstdint>

#include "safeplc/ports/i_clock.hpp"

class FakeClock final : public safeplc::ports::IClock {
public:
    FakeClock() = default;

    [[nodiscard]] std::uint32_t millis() const noexcept override { return now_; }

    void advance(std::uint32_t ms) noexcept { now_ += ms; }
    void set(std::uint32_t ms) noexcept { now_ = ms; }

private:
    std::uint32_t now_ = 0U;
};
