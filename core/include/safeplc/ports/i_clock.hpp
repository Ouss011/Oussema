// SPDX-License-Identifier: MIT
// Abstract clock — injected into time-dependent classes so they can be unit
// tested without sleeping. In production it wraps std::chrono / CLOCK_MONOTONIC.

#pragma once

#include <cstdint>

namespace safeplc::ports {

class IClock {
public:
    IClock() = default;
    virtual ~IClock() = default;

    IClock(const IClock&) = delete;
    IClock& operator=(const IClock&) = delete;
    IClock(IClock&&) = delete;
    IClock& operator=(IClock&&) = delete;

    /// Monotonic time in milliseconds since an arbitrary epoch.
    virtual std::uint32_t millis() const noexcept = 0;
};

}  // namespace safeplc::ports
