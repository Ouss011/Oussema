// SPDX-License-Identifier: MIT
// Ring buffer diagnostic log — fixed capacity, no dynamic allocation.
//
// Stores at most N most-recent entries. Pushing into a full log overwrites
// the oldest. Used to keep recent fault context for post-mortem analysis.

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace safeplc::diag {

enum class DiagSeverity : std::uint8_t { Info, Warning, Error, Critical };

struct DiagEntry {
    std::uint32_t timestamp_ms;
    DiagSeverity severity;
    std::uint16_t code;
};

template <std::size_t N>
class DiagLog {
    static_assert(N >= 2, "DiagLog capacity must be >= 2");

public:
    DiagLog() = default;

    void push(const DiagEntry& e) noexcept {
        buffer_[head_] = e;
        head_ = (head_ + 1U) % N;
        if (size_ < N) {
            ++size_;
        } else {
            // overwriting oldest -> advance tail
            tail_ = (tail_ + 1U) % N;
        }
    }

    [[nodiscard]] std::optional<DiagEntry> pop() noexcept {
        if (size_ == 0U) {
            return std::nullopt;
        }
        DiagEntry e = buffer_[tail_];
        tail_ = (tail_ + 1U) % N;
        --size_;
        return e;
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty()   const noexcept { return size_ == 0U; }
    [[nodiscard]] bool full()    const noexcept { return size_ == N; }
    [[nodiscard]] static constexpr std::size_t capacity() noexcept { return N; }

    void clear() noexcept {
        head_ = 0U;
        tail_ = 0U;
        size_ = 0U;
    }

private:
    std::array<DiagEntry, N> buffer_{};
    std::size_t head_ = 0U;
    std::size_t tail_ = 0U;
    std::size_t size_ = 0U;
};

}  // namespace safeplc::diag
