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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../Widgets" as Widgets

Page {
    id: root

    //
    // Page background
    //
    background: TextField {
        enabled: false
        palette.base: Cpp_ThemeManager.setupPanelBackground
    }

    //
    // Custom properties
    //
    property int group
    readonly property int minSize: 320
    readonly property int maxSize: 380
    readonly property int cellHeight: 480
    readonly property int columns: Math.floor((grid.width - 2 * scroll.width) / cWidth)
    readonly property int cellWidth: cWidth + ((grid.width - 2 * scroll.width) - (cWidth) * columns) / columns
    readonly property int cWidth: Math.min(maxSize, Math.max(minSize, (grid.width - 2 * scroll.width) / grid.model))

    //
    // Connections with JSON editor
    //
    Connections {
        target: Cpp_Project_Model

        function onGroupChanged(id) {
            if (id === group) {
                grid.model = 0
                grid.model = Cpp_Project_Model.datasetCount(group)
                grid.positionViewAtIndex(grid.count - 1, GridView.Beginning)
            }
        }
    }

    //
    // Main layout
    //
    ColumnLayout {
        id: column
        spacing: app.spacing
        anchors.fill: parent
        anchors.margins: app.spacing

        //
        // Notes rectangle
        //
        Rectangle {
            id: notes
            radius: 16
            Layout.fillWidth: true
            color: Cpp_ThemeManager.highlight
            Layout.minimumHeight: 32 + 2 * app.spacing
            visible: widget.currentIndex === 1 || widget.currentIndex === 2

            RowLayout {
                spacing: app.spacing
                anchors.centerIn: parent
                Layout.alignment: Qt.AlignHCenter
                visible: widget.currentIndex === 1

                Widgets.Icon {
                    width: 32
                    height: 32
                    color: palette.highlightedText
                    Layout.alignment: Qt.AlignHCenter
                    source: "qrc:/icons/accelerometer.svg"
                } Label {
                    font.pixelSize: 18
                    wrapMode: Label.WordWrap
                    color: palette.highlightedText
                    text: "<b>" + qsTr("Note:") + "</b> " + qsTr("The accelerometer widget expects values in m/s².")
                }
            }

            RowLayout {
                spacing: app.spacing
                anchors.centerIn: parent
                Layout.alignment: Qt.AlignHCenter
                visible: widget.currentIndex === 2

                Widgets.Icon {
                    width: 32
                    height: 32
                    source: "qrc:/icons/gyro.svg"
                    color: palette.highlightedText
                    Layout.alignment: Qt.AlignHCenter
                } Label {
                    font.pixelSize: 18
                    wrapMode: Label.WordWrap
                    color: palette.highlightedText
                    text: "<b>" + qsTr("Note:") + "</b> " + qsTr("The gyroscope widget expects values in degrees (0° to 360°).")
                }
            }
        }

        //
        // Group title
        //
        RowLayout {
            spacing: app.spacing
            Layout.fillWidth: true

            Label {
                text: qsTr("Title") + ":"
                Layout.alignment: Qt.AlignVCenter
            }

            TextField {
                Layout.minimumWidth: 320
                placeholderText: qsTr("Title")
                Layout.alignment: Qt.AlignVCenter
                text: Cpp_Project_Model.groupTitle(group)

                onTextChanged: {
                    Cpp_Project_Model.setGroupTitle(group, text)
                    root.title = qsTr("Group %1 - %2").arg(group + 1).arg(Cpp_Project_Model.groupTitle(group))
                }
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Group widget") + ":"
                Layout.alignment: Qt.AlignVCenter
            }

            ComboBox {
                id: widget
                Layout.minimumWidth: 180
                Layout.alignment: Qt.AlignVCenter
                model: Cpp_Project_Model.availableGroupLevelWidgets()
                currentIndex: Cpp_Project_Model.groupWidgetIndex(group)
                onCurrentIndexChanged: {
                    var prevIndex = Cpp_Project_Model.groupWidgetIndex(group)
                    if (currentIndex !== prevIndex) {
                        if (!Cpp_Project_Model.setGroupWidget(group, currentIndex))
                            currentIndex = prevIndex
                    }
                }
            }

            RoundButton {
                icon.width: 18
                icon.height: 18
                enabled: group > 0
                opacity: enabled ? 1 : 0.5
                icon.source: "qrc:/icons/up.svg"
                Layout.alignment: Qt.AlignVCenter
                icon.color: Cpp_ThemeManager.text
                onClicked: Cpp_Project_Model.moveGroupUp(group)
            }

            RoundButton {
                icon.width: 18
                icon.height: 18
                opacity: enabled ? 1 : 0.5
                Layout.alignment: Qt.AlignVCenter
                icon.color: Cpp_ThemeManager.text
                icon.source: "qrc:/icons/down.svg"
                enabled: group < Cpp_Project_Model.groupCount - 1
                onClicked: Cpp_Project_Model.moveGroupDown(group)
            }

            RoundButton {
                icon.width: 18
                icon.height: 18
                Layout.alignment: Qt.AlignVCenter
                icon.color: Cpp_ThemeManager.text
                icon.source: "qrc:/icons/delete-item.svg"
                onClicked: Cpp_Project_Model.deleteGroup(group)
            }
        }

        //
        // Datasets
        //
        Item {
            clip: true
            Layout.fillWidth: true
            Layout.fillHeight: true

            //
            // Background
            //
            TextField {
                enabled: false
                anchors.fill: parent
            }

            //
            // Empty group text & icon
            //
            ColumnLayout {
                spacing: app.spacing
                anchors.centerIn: parent
                visible: grid.model === 0

                Widgets.Icon {
                    width: 128
                    height: 128
                    color: Cpp_ThemeManager.text
                    source: "qrc:/icons/group.svg"
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    font.bold: true
                    font.pixelSize: 24
                    text: qsTr("Empty group")
                    Layout.alignment: Qt.AlignHCenter
                }

                Label {
                    opacity: 0.8
                    font.pixelSize: 18
                    Layout.alignment: Qt.AlignHCenter
                    wrapMode: Label.WrapAtWordBoundaryOrAnywhere
                    Layout.maximumWidth: grid.width - 8 * app.spacing
                    text: qsTr("Set group title and click on the \"Add dataset\" button to begin")
                }
            }

            //
            // Grid
            //
            GridView {
                id: grid
                clip: true
                anchors.margins: 1
                anchors.fill: parent
                cellWidth: root.cellWidth
                cellHeight: root.cellHeight
                model: Cpp_Project_Model.datasetCount(group)

                ScrollBar.vertical: ScrollBar {
                    id: scroll
                    policy: ScrollBar.AsNeeded
                }

                delegate: Item {
                    width: grid.cellWidth
                    height: grid.cellHeight

                    Loader {
                        id: loader
                        anchors.fill: parent
                        anchors.margins: app.spacing

                        sourceComponent: JsonDatasetDelegate {
                            dataset: index
                            group: root.group
                            multiplotGroup: widget.currentIndex === 4
                            showGroupWidget: widget.currentIndex > 0 && widget.currentIndex !== 4
                        }
                    }
                }
            }
        }

        //
        // Add dataset button
        //
        Button {
            icon.width: 24
            icon.height: 24
            Layout.fillWidth: true
            text: qsTr("Add dataset")
            icon.source: "qrc:/icons/add.svg"
            icon.color: Cpp_ThemeManager.menubarText
            palette.buttonText: Cpp_ThemeManager.menubarText
            palette.button: Cpp_ThemeManager.toolbarGradient1
            palette.window: Cpp_ThemeManager.toolbarGradient1
            visible: widget.currentIndex === 0 || widget.currentIndex === 4
            onClicked: {
                Cpp_Project_Model.addDataset(group)
                grid.positionViewAtIndex(grid.count - 1, GridView.Beginning)
            }
        }
    }
}
