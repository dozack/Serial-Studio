cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

project(
    serial-studio
    VERSION 0.9.7.9
    LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -pipe -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -pipe -g3 -Og")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(
    Qt6 REQUIRED
    COMPONENTS Core
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
               Xml)

qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/QMapControl)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/QRealFourier)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/QSourceHighlite)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/qwt)

set(QTCSV_STATIC_LIB ON)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/libs/qtcsv)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
            ${CMAKE_CURRENT_LIST_DIR}/src/CSV
            ${CMAKE_CURRENT_LIST_DIR}/src/IO
            ${CMAKE_CURRENT_LIST_DIR}/src/IO/Drivers
            ${CMAKE_CURRENT_LIST_DIR}/src/JSON
            ${CMAKE_CURRENT_LIST_DIR}/src/Misc
            ${CMAKE_CURRENT_LIST_DIR}/src/Plugins
            ${CMAKE_CURRENT_LIST_DIR}/src/Project
            ${CMAKE_CURRENT_LIST_DIR}/src/UI
            ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets
            ${CMAKE_CURRENT_LIST_DIR}/src/UI/Widgets/Common)

file(GLOB_RECURSE SERIALSTUDIO_MOC_HDRS ${CMAKE_CURRENT_LIST_DIR}/src/**/*.h)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        ${SERIALSTUDIO_MOC_HDRS}
        ${CMAKE_CURRENT_LIST_DIR}/assets/Resources.qrc
        ${CMAKE_CURRENT_LIST_DIR}/src/CSV/Export.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/CSV/Player.cpp
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
        ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE Qt::Core
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
            ssl)

# If QSU is enabled, add QSU_INCLUDE_MOC=1
target_compile_definitions(
    ${PROJECT_NAME} PRIVATE DISABLE_QSU SERIAL_STUDIO_INCLUDE_MOC
                            _USE_MATH_DEFINES)

# Resources:

# set_source_files_properties("libs/QSimpleUpdater/etc/resources/update.png"
# PROPERTIES QT_RESOURCE_ALIAS "update.png") set(qsimpleupdater_resource_files
# "libs/QSimpleUpdater/etc/resources/update.png")

# qt_add_resources( ${PROJECT_NAME} "qsimpleupdater" PREFIX "/icons" BASE
# "libs/QSimpleUpdater/etc/resources" FILES ${qsimpleupdater_resource_files})

# set(Resources_resource_files "assets/fonts/Roboto-Bold.ttf"
# "assets/fonts/Roboto-Regular.ttf" "assets/fonts/RobotoMono-Bold.ttf"
# "assets/fonts/RobotoMono-Regular.ttf" "assets/icons/ac-power.svg"
# "assets/icons/accelerometer.svg" "assets/icons/add.svg"
# "assets/icons/apply.svg" "assets/icons/arrow-right.svg"
# "assets/icons/backspace.svg" "assets/icons/bar.svg" "assets/icons/battery.svg"
# "assets/icons/bug.svg" "assets/icons/close.svg" "assets/icons/code.svg"
# "assets/icons/compass.svg" "assets/icons/connect.svg" "assets/icons/copy.svg"
# "assets/icons/cut.svg" "assets/icons/dashboard.svg" "assets/icons/dataset.svg"
# "assets/icons/delete-item.svg" "assets/icons/delete.svg"
# "assets/icons/developer-board.svg" "assets/icons/device-hub.svg"
# "assets/icons/disconnect.svg" "assets/icons/down.svg"
# "assets/icons/drag-drop.svg" "assets/icons/end-sequence.svg"
# "assets/icons/error.svg" "assets/icons/ethernet.svg" "assets/icons/expand.svg"
# "assets/icons/fft.svg" "assets/icons/fullscreen-exit.svg"
# "assets/icons/fullscreen.svg" "assets/icons/gauge.svg" "assets/icons/gps.svg"
# "assets/icons/graphs.svg" "assets/icons/group.svg" "assets/icons/gyro.svg"
# "assets/icons/heart-broken.svg" "assets/icons/help.svg"
# "assets/icons/hide-all.svg" "assets/icons/info.svg" "assets/icons/json.svg"
# "assets/icons/led.svg" "assets/icons/link.svg" "assets/icons/location-off.svg"
# "assets/icons/location-on.svg" "assets/icons/map.svg"
# "assets/icons/media-next.svg" "assets/icons/media-pause.svg"
# "assets/icons/media-play.svg" "assets/icons/media-prev.svg"
# "assets/icons/multiplot.svg" "assets/icons/new.svg" "assets/icons/open.svg"
# "assets/icons/paste.svg" "assets/icons/plot.svg" "assets/icons/points.svg"
# "assets/icons/power.svg" "assets/icons/ram.svg" "assets/icons/redo.svg"
# "assets/icons/refresh.svg" "assets/icons/registration.svg"
# "assets/icons/restore.svg" "assets/icons/right.svg" "assets/icons/save.svg"
# "assets/icons/schedule.svg" "assets/icons/scroll-bottom.svg"
# "assets/icons/scroll-down.svg" "assets/icons/scroll-top.svg"
# "assets/icons/scroll-up.svg" "assets/icons/search.svg" "assets/icons/send.svg"
# "assets/icons/separator.svg" "assets/icons/settings.svg"
# "assets/icons/show-all.svg" "assets/icons/start-sequence.svg"
# "assets/icons/template.svg" "assets/icons/thermometer.svg"
# "assets/icons/undo.svg" "assets/icons/up.svg" "assets/icons/update.svg"
# "assets/icons/usb.svg" "assets/icons/visibility.svg"
# "assets/icons/warning.svg" "assets/icons/widget.svg"
# "assets/images/donate-qr.svg" "assets/images/icon-small@1x.png"
# "assets/images/icon-small@2x.png" "assets/images/icon-window.svg"
# "assets/images/icon@1x.png" "assets/images/icon@2x.png"
# "assets/images/shadow.png" "assets/images/window-shadow.png"
# "assets/messages/Acknowledgements.txt" "assets/messages/Welcome_DE.txt"
# "assets/messages/Welcome_EN.txt" "assets/messages/Welcome_ES.txt"
# "assets/messages/Welcome_RU.txt" "assets/messages/Welcome_ZH.txt"
# "assets/qml/Dashboard/DashboardTitle.qml"
# "assets/qml/Dashboard/ViewOptions.qml"
# "assets/qml/Dashboard/ViewOptionsDelegate.qml"
# "assets/qml/Dashboard/WidgetDelegate.qml"
# "assets/qml/Dashboard/WidgetGrid.qml" "assets/qml/Dashboard/WidgetModel.qml"
# "assets/qml/FramelessWindow/CustomWindow.qml"
# "assets/qml/FramelessWindow/ResizeHandles.qml"
# "assets/qml/FramelessWindow/Titlebar.qml"
# "assets/qml/FramelessWindow/WindowButton.qml"
# "assets/qml/FramelessWindow/WindowButtonMacOS.qml"
# "assets/qml/Panes/Console.qml" "assets/qml/Panes/Dashboard.qml"
# "assets/qml/Panes/Setup.qml" "assets/qml/Panes/SetupPanes/Devices/Network.qml"
# "assets/qml/Panes/SetupPanes/Devices/Serial.qml"
# "assets/qml/Panes/SetupPanes/Hardware.qml"
# "assets/qml/Panes/SetupPanes/Settings.qml" "assets/qml/Panes/Toolbar.qml"
# "assets/qml/PlatformDependent/DecentMenuItem.qml"
# "assets/qml/PlatformDependent/Menubar.qml"
# "assets/qml/PlatformDependent/MenubarMacOS.qml"
# "assets/qml/ProjectEditor/Footer.qml"
# "assets/qml/ProjectEditor/GroupEditor.qml"
# "assets/qml/ProjectEditor/Header.qml"
# "assets/qml/ProjectEditor/JsonDatasetDelegate.qml"
# "assets/qml/ProjectEditor/JsonGroupDelegate.qml"
# "assets/qml/ProjectEditor/TreeView.qml" "assets/qml/Widgets/Icon.qml"
# "assets/qml/Widgets/JSONDropArea.qml" "assets/qml/Widgets/Shadow.qml"
# "assets/qml/Widgets/Terminal.qml" "assets/qml/Widgets/Window.qml"
# "assets/qml/Windows/About.qml" "assets/qml/Windows/Acknowledgements.qml"
# "assets/qml/Windows/CsvPlayer.qml" "assets/qml/Windows/Donate.qml"
# "assets/qml/Windows/MainWindow.qml" "assets/qml/Windows/ProjectEditor.qml"
# "assets/qml/main.qml" "assets/scripts/frame-parser.js"
# "assets/themes/1_Flat.json" "assets/themes/2_Dark.json"
# "assets/themes/3_Classic.json" "assets/themes/4_Midnight.json"
# "assets/themes/5_Noir.json" "assets/themes/6_Mathworks.json"
# "assets/touchbar/console.png" "assets/touchbar/dashboard.png"
# "assets/touchbar/setup.png" "assets/translations/de.qm"
# "assets/translations/de.ts" "assets/translations/en.qm"
# "assets/translations/en.ts" "assets/translations/es.qm"
# "assets/translations/es.ts" "assets/translations/ru.qm"
# "assets/translations/ru.ts" "assets/translations/zh.qm"
# "assets/translations/zh.ts" "assets/window-border/close.svg"
# "assets/window-border/fullscreen.svg"
# "assets/window-border/macOS/close-active.svg"
# "assets/window-border/macOS/close-hover.svg"
# "assets/window-border/macOS/close-normal.svg"
# "assets/window-border/macOS/maximize-active.svg"
# "assets/window-border/macOS/maximize-hover.svg"
# "assets/window-border/macOS/maximize-normal.svg"
# "assets/window-border/macOS/minimize-active.svg"
# "assets/window-border/macOS/minimize-hover.svg"
# "assets/window-border/macOS/minimize-normal.svg"
# "assets/window-border/maximize.svg" "assets/window-border/minimize.svg"
# "assets/window-border/restore.svg" "assets/window-border/unmaximize.svg")

# qt_add_resources( ${PROJECT_NAME} "Resources" PREFIX "/" BASE "assets" FILES
# ${Resources_resource_files})
