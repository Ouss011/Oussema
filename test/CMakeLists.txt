# GoogleTest is fetched automatically — no apt-get needed.
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    URL_HASH SHA256=8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7
)

# Silence GoogleTest warnings under our strict flags.
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# Common library for fakes shared by multiple tests.
add_library(safeplc_test_fakes INTERFACE)
target_include_directories(safeplc_test_fakes INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/fakes)
target_link_libraries(safeplc_test_fakes INTERFACE safeplc_core)

function(safeplc_add_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name} PRIVATE
        safeplc_core
        safeplc_test_fakes
        GTest::gtest
        GTest::gtest_main
        GTest::gmock)
    gtest_discover_tests(${name})
endfunction()

safeplc_add_test(test_voter_1oo2)
safeplc_add_test(test_voter_2oo3)
safeplc_add_test(test_estop)
safeplc_add_test(test_two_hand)
safeplc_add_test(test_watchdog)
safeplc_add_test(test_safety_fsm)
safeplc_add_test(test_diag_log)
