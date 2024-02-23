/*
 * Copyright (c) 2020-2023 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import QtCore
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../Widgets" as Widgets
import "SetupPanes" as SetupPanes

Item {
    id: root

    //
    // Custom properties
    //
    property int setupMargin: 0
    property int displayedWidth: 380 + app.spacing * 1.5
    readonly property int maxItemWidth: column.width - 2 * spacing

    //
    // Displays the setup panel
    //
    function show() {
        setupMargin = 0
    }

    //
    // Hides the setup panel
    //
    function hide() {
        setupMargin = -1 * displayedWidth
    }

    //
    // Animations
    //
    visible: setupMargin > -1 * displayedWidth
    Behavior on setupMargin {NumberAnimation{}}

    //
    // Save settings
    //
    Settings {
        //
        // Misc settings
        //
        property alias auto: commAuto.checked
        property alias manual: commManual.checked
        property alias tabIndex: tab.currentIndex
        property alias csvExport: csvLogging.checked

        //
        // App settings
        //
        property alias language: settings.language
        property alias tcpPlugins: settings.tcpPlugins
        property alias windowShadows: settings.windowShadows
    }

    //
    // Update manual/auto checkboxes
    //
    Connections {
        target: Cpp_JSON_Generator
        function onOperationModeChanged() {
            commAuto.checked = (Cpp_JSON_Generator.operationMode === 1)
            commManual.checked = (Cpp_JSON_Generator.operationMode === 0)
        }
    }

    //
    // Window
    //
    Widgets.Window {
        gradient: true
        title: qsTr("Setup")
        anchors.fill: parent
        anchors.leftMargin: 0
        headerDoubleClickEnabled: false
        icon.source: "qrc:/icons/settings.svg"
        anchors.margins: (app.spacing * 1.5) - 5
        backgroundColor: Cpp_ThemeManager.paneWindowBackground

        //
        // Control arrangement
        //
        ColumnLayout {
            id: column
            anchors.fill: parent
            spacing: app.spacing / 2
            anchors.margins: app.spacing * 1.5

            //
            // Comm mode selector
            //
            Label {
                font.bold: true
                text: qsTr("Communication Mode") + ":"
            } RadioButton {
                id: commAuto
                checked: true
                Layout.maximumWidth: root.maxItemWidth
                text: qsTr("No parsing (device sends JSON data)")
                onCheckedChanged: {
                    if (checked)
                        Cpp_JSON_Generator.setOperationMode(1)
                    else
                        Cpp_JSON_Generator.setOperationMode(0)
                }
            } RadioButton {
                id: commManual
                checked: false
                Layout.maximumWidth: root.maxItemWidth
                text: qsTr("Parse via JSON project file")
                onCheckedChanged: {
                    if (checked)
                        Cpp_JSON_Generator.setOperationMode(0)
                    else
                        Cpp_JSON_Generator.setOperationMode(1)
                }
            }

            //
            // Map file selector button
            //
            Button {
                Layout.fillWidth: true
                opacity: enabled ? 1 : 0.5
                enabled: commManual.checked
                Layout.maximumWidth: root.maxItemWidth
                onClicked: Cpp_JSON_Generator.loadJsonMap()
                text: (Cpp_JSON_Generator.jsonMapFilename.length ? qsTr("Change project file (%1)").arg(Cpp_JSON_Generator.jsonMapFilename) :
                                                                   qsTr("Select project file") + "...")
            }

            //
            // Spacer
            //
            Item {
                height: app.spacing / 2
            }

            //
            // Enable/disable CSV logging
            //
            RowLayout {
                Layout.fillWidth: true

                Switch {
                    id: csvLogging
                    text: qsTr("Create CSV file")
                    Layout.alignment: Qt.AlignVCenter
                    checked: Cpp_CSV_Export.exportEnabled
                    Layout.maximumWidth: root.maxItemWidth
                    palette.highlight: Cpp_ThemeManager.csvCheckbox

                    onCheckedChanged:  {
                        if (Cpp_CSV_Export.exportEnabled !== checked)
                            Cpp_CSV_Export.exportEnabled = checked
                    }
                }

                Item {
                    Layout.fillWidth: true
                }

                RoundButton {
                    icon.width: 24
                    icon.height: 24
                    icon.color: Cpp_ThemeManager.text
                    Layout.alignment: Qt.AlignVCenter
                    icon.source: "qrc:/icons/help.svg"
                    onClicked: Qt.openUrlExternally("https://github.com/Serial-Studio/Serial-Studio/wiki")
                }
            }

            //
            // Spacer
            //
            Item {
                height: app.spacing / 2
            }

            //
            // Tab bar
            //
            TabBar {
                height: 24
                id: tab
                Layout.fillWidth: true
                Layout.maximumWidth: root.maxItemWidth

                TabButton {
                    text: qsTr("Device")
                    height: tab.height + 3
                    width: implicitWidth + 2 * app.spacing
                }

                TabButton {
                    text: qsTr("Settings")
                    height: tab.height + 3
                    width: implicitWidth + 2 * app.spacing
                }
            }

            //
            // Tab bar contents
            //
            StackLayout {
                id: stack
                clip: true
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: tab.currentIndex
                Layout.topMargin: -parent.spacing - 1

                SetupPanes.Hardware {
                    id: hardware
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    background: TextField {
                        enabled: false
                        palette.base: Cpp_ThemeManager.setupPanelBackground
                    }
                }

                SetupPanes.Settings {
                    id: settings
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    background: TextField {
                        enabled: false
                        palette.base: Cpp_ThemeManager.setupPanelBackground
                    }
                }
            }
        }
    }
}
