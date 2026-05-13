add_executable(safeplc_app
    main.cpp
    linux_clock.cpp)

target_include_directories(safeplc_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(safeplc_app PRIVATE safeplc_core)
