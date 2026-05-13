// SPDX-License-Identifier: MIT
#include "linux_clock.hpp"

#include <chrono>

std::uint32_t LinuxClock::millis() const noexcept {
    using namespace std::chrono;
    const auto now = steady_clock::now().time_since_epoch();
    return static_cast<std::uint32_t>(duration_cast<milliseconds>(now).count());
}
