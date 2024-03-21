cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

add_executable(canbus-print)

target_include_directories(canbus-print
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../../src
    )

target_sources(canbus-print
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../src/IO/HAL_Driver.h
    ${CMAKE_CURRENT_LIST_DIR}/../../src/IO/Drivers/CanBus.h 
    ${CMAKE_CURRENT_LIST_DIR}/../../src/IO/Drivers/CanBus.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/../../src/Misc/TimerEvents.h 
    ${CMAKE_CURRENT_LIST_DIR}/../../src/Misc/TimerEvents.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    )

target_link_libraries(canbus-print
    PRIVATE
    Qt::Widgets
    Qt::SerialBus
    )

target_compile_definitions(canbus-print
    PRIVATE
    SERIAL_STUDIO_INCLUDE_MOC
    )