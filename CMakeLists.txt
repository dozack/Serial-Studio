cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(serial-studio VERSION 0.9.7.9 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD            11 )
set(CMAKE_C_STANDARD_REQUIRED   ON )
set(CMAKE_C_EXTENSIONS          OFF)

set(CMAKE_CXX_STANDARD          11 )
set(CMAKE_CXX_STANDARD_REQUIRED ON )
set(CMAKE_CXX_EXTENSIONS        OFF)

set(CMAKE_INCLUDE_CURRENT_DIR   ON )

message(STATUS "Provided Qt installation: ${QT_INSTALL_DIR}")

find_package(Qt6 
    REQUIRED 
    PATHS ${QT_INSTALL_DIR}
    COMPONENTS 
    Core
    Concurrent
    Core5Compat
    Gui
    Network
    PrintSupport
    Quick
    QuickControls2
    OpenGL
    OpenGLWidgets
    SerialBus
    SerialPort
    Svg
    Widgets
    Xml
    )

qt_standard_project_setup()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/QMapControl)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/QRealFourier)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/QSourceHighlite)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/qwt)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/qtcsv)

file(GLOB_RECURSE SERIALSTUDIO_MOC_HDRS ${CMAKE_CURRENT_LIST_DIR}/src/**/*.h)

qt_add_resources(SERIALSTUDIO_QRC_SRCS ${CMAKE_CURRENT_LIST_DIR}/assets/Resources.qrc)

qt_add_executable(${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}/src
    ${CMAKE_CURRENT_LIST_DIR}/src/CSV
    ${CMAKE_CURRENT_LIST_DIR}/src/DBC
    ${CMAKE_CURRENT_LIST_DIR}/src/IO
    ${CMAKE_CURRENT_LIST_DIR}/src/IO/Drivers
    ${CMAKE_CURRENT_LIST_DIR}/src/JSON
    ${CMAKE_CURRENT_LIST_DIR}/src/Misc
    ${CMAKE_CURRENT_LIST_DIR}/src/Plugins
    ${CMAKE_CURRENT_LIST_DIR}/src/Project
    ${CMAKE_CURRENT_LIST_DIR}/src/UI
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Common
    )

target_precompile_headers(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/DataTypes.h
    )

target_sources(${PROJECT_NAME}
    PRIVATE
    ${SERIALSTUDIO_MOC_HDRS}
    ${SERIALSTUDIO_QRC_SRCS}
    ${CMAKE_CURRENT_LIST_DIR}/src/CSV/Export.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/CSV/Player.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/DBC/Loader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/IO/Checksum.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/IO/Console.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/IO/Drivers/CanBus.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/IO/Drivers/Network.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/IO/Drivers/Serial.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/IO/Manager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/JSON/Dataset.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/JSON/Frame.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/JSON/Generator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/JSON/Group.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Misc/MacExtras.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Misc/ModuleManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Misc/ThemeManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Misc/TimerEvents.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Misc/Translator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Misc/Utilities.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Plugins/Server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Project/CodeEditor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Project/Model.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Dashboard.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/DashboardWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/DeclarativeWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Accelerometer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Bar.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Common/AnalogGauge.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Common/AttitudeIndicator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Common/BaseWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Common/ElidedLabel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Common/KLed.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Compass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/DataGroup.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/FFTPlot.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/GPS.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Gauge.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Gyroscope.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/LEDPanel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/MultiPlot.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Plot.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Terminal.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::PrintSupport
    Qt::Quick
    Qt::QuickControls2
    Qt::SerialBus
    Qt::SerialPort
    Qt::Svg
    Qt::Widgets
    Qt::Xml
    Qt::Core5Compat
    Qt::Concurrent
    Qt::PrintSupport
    Qt::Svg
    qwt
    qtcsv
    qmapcontrol
    qrealfourier
    qsourcehighlite
    crypto
    ssl
    )

# If QSU is enabled, add QSU_INCLUDE_MOC=1
target_compile_definitions(${PROJECT_NAME} 
    PRIVATE 
    DISABLE_QSU 
    SERIAL_STUDIO_INCLUDE_MOC
    _USE_MATH_DEFINES
    )

target_compile_options(${PROJECT_NAME}
    PRIVATE
    -pipe
    -Wall
    -Wextra
    -pedantic
    )