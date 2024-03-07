cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

add_library(qwt STATIC)

target_include_directories(qwt PUBLIC ${CMAKE_CURRENT_LIST_DIR}/classincludes
                                      ${CMAKE_CURRENT_LIST_DIR}/src)

file(GLOB QWT_SRCS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)

target_sources(qwt PRIVATE ${QWT_SRCS})

target_link_libraries(qwt PRIVATE Qt::Concurrent Qt::PrintSupport Qt::Svg
                                  Qt::OpenGL Qt::OpenGLWidgets)

target_compile_definitions(qwt PRIVATE QWT_MOC_INCLUDE=1)
