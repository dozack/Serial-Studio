cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(qmapcontrol SHARED)

target_include_directories(qmapcontrol 
    PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/src
    )

file(GLOB QMAPCONTROL_SRCS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

target_sources(qmapcontrol 
    PRIVATE 
    ${QMAPCONTROL_SRCS}
    )

target_link_libraries(qmapcontrol 
    PRIVATE 
    Qt::Core5Compat 
    Qt::Gui 
    Qt::Network
    Qt::Widgets
    )
