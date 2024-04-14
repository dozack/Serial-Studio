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

Control {
    id: root

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: app.spacing

        GridLayout {
            id: layout
            columns: 2
            Layout.fillWidth: true
            rowSpacing: app.spacing
            columnSpacing: app.spacing

            Label {
                opacity: enabled ? 1 : 0.5
                text: qsTr("CAN interface") + ":"
                enabled: !Cpp_IO_Manager.connected
            } 
            
            ComboBox {
                id: _interfaceCombo
                Layout.fillWidth: true
                opacity: enabled ? 1 : 0.5
                model: Cpp_IO_CanBus.availableDevices
                enabled: !Cpp_IO_Manager.connected
                currentIndex: Cpp_IO_CanBus.interfaceIndex
                palette.base: Cpp_ThemeManager.setupPanelBackground
                onCurrentIndexChanged: {
                    if (currentIndex !== Cpp_IO_CanBus.interfaceIndex) {
                        Cpp_IO_CanBus.interfaceIndex = currentIndex
                    }
                }
            }

            Label {
                opacity: enabled ? 1 : 0.5
                text: qsTr("Database file") + ":"
                enabled: !Cpp_IO_Manager.connected
            } 

            Button {
                Layout.fillWidth: true
                opacity: enabled ? 1 : 0.5
                enabled: !Cpp_IO_Manager.connected
                onClicked: Cpp_DBC_Loader.dbcFileLoad()
                text: (Cpp_DBC_Loader.dbcFileName.length ? Cpp_DBC_Loader.dbcFileName :
                                                           qsTr("Select DBC file"))
            }            
        }

        Item {
            Layout.fillHeight: true
        }
    }
}