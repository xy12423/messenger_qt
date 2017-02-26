import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.folderlistmodel 2.1

Item {
    id: item_pagefile
    width: 320
    height: 480
    property alias button_back: button_back
    property alias button_ok_file: button_ok_file
    property alias button_cancel_file: button_cancel_file
    property alias listView_file: listView_file
    property alias listModel_file: listModel_file
    property alias text_path: text_path
    property int selectedIndex
    property url folderIcon
    property url fileIcon

    ColumnLayout {
        id: columnLayout_pagefile
        anchors.fill: parent

        RowLayout {
            id: rowLayout_header
            width: parent.width
            height: 25
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredHeight: 25

            Button {
                id: button_back
                width: parent.height
                height: parent.height
                Layout.fillHeight: true
                Layout.fillWidth: true
                Layout.maximumWidth: parent.height
                spacing: 5

                text: qsTr("<")
            }

            Rectangle {
                id: rectangle_path
                height: parent.height
                Layout.fillHeight: true
                Layout.fillWidth: true
                color: "#cccccc"

                Text {
                    id: text_path
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 0

                    font.pixelSize: 12
                }
            }
        }

        ListView {
            id: listView_file
            width: parent.width
            height: 360
            orientation: ListView.Vertical
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredHeight: 360
            currentIndex: selectedIndex

            model: FolderListModel {
                id: listModel_file
                showDirsFirst: true
            }

            delegate: Item {
                id: item_listViewItem
                width: parent.width
                height: 20
                RowLayout
                {
                    anchors.fill: parent
                    Image {
                        id: image_listViewItem
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        Layout.fillHeight: true

                        source: fileIsDir ? folderIcon : fileIcon
                    }
                    Text {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        anchors.left: image_listViewItem.right
                        anchors.leftMargin: 5
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.verticalCenterOffset: 0

                        text: fileName
                        verticalAlignment: TextField.AlignVCenter
                    }
                }
                MouseArea {
                    id: mouse_listViewItem
                    anchors.fill: parent

                    onClicked: selectedIndex = index
                }
            }
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            focus: true
        }

        RowLayout {
            id: rowLayout_footer
            width: parent.width
            height: 40
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredHeight: 40

            Button {
                id: button_ok_file
                height: parent.height
                text: qsTr("OK")
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5
            }

            Button {
                id: button_cancel_file
                height: parent.height
                text: qsTr("Cancel")
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5
            }
        }
    }
}
