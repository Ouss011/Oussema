# Coverage instrumentation for GCC (gcov / lcov).
# Activated when -DSAFEPLC_COVERAGE=ON (see CMakeLists.txt root).

add_library(project_coverage INTERFACE)

if(SAFEPLC_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(WARNING "SAFEPLC_COVERAGE is ON but compiler is not GCC — gcov may not work as expected.")
    endif()
    target_compile_options(project_coverage INTERFACE
        --coverage -O0 -g -fno-inline -fno-inline-small-functions -fno-default-inline)
    target_link_options(project_coverage INTERFACE --coverage)
endif()
