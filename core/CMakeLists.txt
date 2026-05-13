# Core safety library — pure logic, no I/O dependency.
# Header-only modules are exposed via the INTERFACE target.

add_library(safeplc_core INTERFACE)

target_include_directories(safeplc_core INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(safeplc_core INTERFACE
    project_warnings
    project_coverage)

target_compile_features(safeplc_core INTERFACE cxx_std_17)
