cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(qsourcehighlite SHARED)

target_include_directories(qsourcehighlite 
    PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}
    )

file(GLOB QSOURCEHIGHLITE_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

target_sources(qsourcehighlite 
    PRIVATE 
    ${QSOURCEHIGHLITE_SRCS}
    )

target_link_libraries(qsourcehighlite 
    PRIVATE 
    Qt::Gui
    )
