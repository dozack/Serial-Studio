cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(qrealfourier SHARED)

target_include_directories(qrealfourier 
    PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/fftreal
    ${CMAKE_CURRENT_LIST_DIR}/headers
    )

file(GLOB QREALFOURIER_SRCS ${CMAKE_CURRENT_LIST_DIR}/sources/*.cpp)

target_sources(qrealfourier 
    PRIVATE 
    ${QREALFOURIER_SRCS}
    )

target_link_libraries(qrealfourier 
    PRIVATE 
    Qt::Core
    )
