# CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(polonius-gui LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    Slint
    GIT_REPOSITORY https://github.com/slint-ui/slint.git
    # `release/1` will auto-upgrade to the latest Slint >= 1.0.0 and < 2.0.0
    # `release/1.0` will auto-upgrade to the latest Slint >= 1.0.0 and < 1.1.0
    GIT_TAG release/1
    SOURCE_SUBDIR api/cpp
)
FetchContent_MakeAvailable(Slint)

add_executable(polonius-gui main.cpp)
target_link_libraries(polonius-gui PRIVATE Slint::Slint)
slint_target_sources(polonius-gui polonius-gui.slint)
# On Windows, copy the Slint DLL next to the application binary so that it's found.
if (WIN32)
    add_custom_command(TARGET polonius-gui POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:polonius-gui> $<TARGET_FILE_DIR:polonius-gui> COMMAND_EXPAND_LISTS)
endif()
