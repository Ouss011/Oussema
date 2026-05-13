// SPDX-License-Identifier: MIT
#pragma once

#include "safeplc/ports/i_clock.hpp"

class LinuxClock final : public safeplc::ports::IClock {
public:
    [[nodiscard]] std::uint32_t millis() const noexcept override;
};
