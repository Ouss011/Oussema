// SPDX-License-Identifier: MIT
// Tiny demo daemon that wires the safety building blocks together.
// Run with no arguments: prints the safety FSM state every 100 ms.
// Press Ctrl-C to exit.

#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

#include "linux_clock.hpp"
#include "safeplc/fsm/safety_fsm.hpp"
#include "safeplc/safety/estop.hpp"
#include "safeplc/safety/voter_2oo3.hpp"
#include "safeplc/safety/watchdog.hpp"

namespace {

std::atomic<bool> g_running{true};

void on_sigint(int /*sig*/) noexcept { g_running.store(false); }

const char* state_name(safeplc::fsm::SafetyState s) noexcept {
    switch (s) {
    case safeplc::fsm::SafetyState::Init:   return "Init";
    case safeplc::fsm::SafetyState::PreRun: return "PreRun";
    case safeplc::fsm::SafetyState::Run:    return "Run";
    case safeplc::fsm::SafetyState::Fault:  return "Fault";
    }
    return "?";
}

}  // namespace

int main() {
    std::signal(SIGINT, on_sigint);

    LinuxClock clock;
    safeplc::fsm::SafetyFsm fsm;
    safeplc::safety::Voter2oo3 voter(3U);
    safeplc::safety::EStop estop;
    safeplc::safety::Watchdog wd(clock, 500U);

    // Boot sequence.
    fsm.on_event(safeplc::fsm::SafetyEvent::InitOk);
    fsm.on_event(safeplc::fsm::SafetyEvent::RunRequest);

    std::cout << "safeplc-mini starting — Ctrl-C to stop\n";

    std::uint32_t tick = 0U;
    while (g_running.load()) {
        // Pretend redundant channel reads.
        const bool ch_a = true;
        const bool ch_b = true;
        const bool ch_c = ((tick % 50U) != 49U);  // brief 1-tick glitch every 5 s

        const auto v = voter.vote(ch_a, ch_b, ch_c);
        if (v.fault == safeplc::safety::Voter2oo3::FaultMode::Discrepancy) {
            fsm.on_event(safeplc::fsm::SafetyEvent::Fault);
        }

        const bool estop_triggered = estop.update(true);  // NC, always safe in demo
        if (estop_triggered) {
            fsm.on_event(safeplc::fsm::SafetyEvent::Fault);
        }

        wd.kick();

        if ((tick % 10U) == 0U) {
            std::cout << "[t=" << clock.millis() << "ms] state=" << state_name(fsm.state())
                      << " v=" << v.value << " wd_ok=" << !wd.expired() << '\n';
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        ++tick;
    }

    std::cout << "safeplc-mini stopped.\n";
    return 0;
}
