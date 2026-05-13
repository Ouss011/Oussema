cmake_minimum_required(VERSION 3.20)
project(safeplc_mini
        VERSION 0.1.0
        DESCRIPTION "Minimal soft-PLC with safety functions — interview prep"
        LANGUAGES CXX)

# -----------------------------------------------------------------------------
# Project options
# -----------------------------------------------------------------------------
option(SAFEPLC_BUILD_TESTS "Build unit tests" ON)
option(SAFEPLC_BUILD_APP   "Build main daemon" ON)
option(SAFEPLC_COVERAGE    "Enable code coverage (GCC + lcov)" OFF)
option(SAFEPLC_ASAN        "Enable AddressSanitizer + UBSan"   OFF)

# -----------------------------------------------------------------------------
# Global C++ settings
# -----------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Helpers (warnings, coverage flags).
include(cmake/Warnings.cmake)
include(cmake/Coverage.cmake)

# -----------------------------------------------------------------------------
# Sanitizers
# -----------------------------------------------------------------------------
if(SAFEPLC_ASAN)
    message(STATUS "Sanitizers ON: ASan + UBSan")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# -----------------------------------------------------------------------------
# Targets
# -----------------------------------------------------------------------------
add_subdirectory(core)

if(SAFEPLC_BUILD_APP)
    add_subdirectory(app)
endif()

if(SAFEPLC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

# -----------------------------------------------------------------------------
# Summary
# -----------------------------------------------------------------------------
message(STATUS "")
message(STATUS "===== safeplc-mini configuration =====")
message(STATUS "  C++ standard       : ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build type         : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Build tests        : ${SAFEPLC_BUILD_TESTS}")
message(STATUS "  Build app          : ${SAFEPLC_BUILD_APP}")
message(STATUS "  Coverage           : ${SAFEPLC_COVERAGE}")
message(STATUS "  Sanitizers (ASan)  : ${SAFEPLC_ASAN}")
message(STATUS "======================================")
message(STATUS "")
