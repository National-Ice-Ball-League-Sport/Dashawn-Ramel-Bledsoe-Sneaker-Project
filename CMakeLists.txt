cmake_minimum_required(VERSION 3.16)
project(FutureStepCpp VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(futurestep_cpp
    cpp/src/telemetry.cpp
    cpp/src/ble_service.cpp
)

target_include_directories(futurestep_cpp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
)

add_executable(futurestep_demo cpp/examples/demo.cpp)
target_link_libraries(futurestep_demo PRIVATE futurestep_cpp)

option(FUTURESTEP_BUILD_FIRMWARE_WRAPPER "Build the C++ firmware wrapper" OFF)
if(FUTURESTEP_BUILD_FIRMWARE_WRAPPER)
    add_library(futurestep_firmware cpp/src/firmware.cpp kalman_jump.c main.c)
    target_include_directories(futurestep_firmware PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/cpp/include
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()
