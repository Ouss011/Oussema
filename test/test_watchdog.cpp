// SPDX-License-Identifier: MIT
#include "safeplc/safety/watchdog.hpp"

#include <gtest/gtest.h>

#include "fakes/fake_clock.hpp"

using safeplc::safety::Watchdog;

class WatchdogTest : public ::testing::Test {
protected:
    FakeClock clock;
};

TEST_F(WatchdogTest, NotExpiredInitially) {
    Watchdog wd(clock, 100U);
    EXPECT_FALSE(wd.expired());
}

TEST_F(WatchdogTest, ExpiresAfterTimeout) {
    Watchdog wd(clock, 100U);
    clock.advance(101U);
    EXPECT_TRUE(wd.expired());
}

TEST_F(WatchdogTest, DoesNotExpireExactlyAtTimeout) {
    Watchdog wd(clock, 100U);
    clock.advance(100U);
    EXPECT_FALSE(wd.expired());
}

TEST_F(WatchdogTest, KickPreventsExpiration) {
    Watchdog wd(clock, 100U);
    for (int i = 0; i < 10; ++i) {
        clock.advance(50U);
        wd.kick();
        EXPECT_FALSE(wd.expired());
    }
}

TEST_F(WatchdogTest, ExpiredStaysStickyEvenAfterKick) {
    Watchdog wd(clock, 100U);
    clock.advance(150U);
    EXPECT_TRUE(wd.expired());
    wd.kick();
    EXPECT_TRUE(wd.expired());  // sticky
}

TEST_F(WatchdogTest, ResetClearsExpiredFlag) {
    Watchdog wd(clock, 100U);
    clock.advance(150U);
    EXPECT_TRUE(wd.expired());
    wd.reset();
    EXPECT_FALSE(wd.expired());
}

TEST_F(WatchdogTest, TimeoutAccessorReturnsConstructorValue) {
    Watchdog wd(clock, 250U);
    EXPECT_EQ(wd.timeout_ms(), 250U);
}
