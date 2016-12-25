import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0
import Qt.labs.folderlistmodel 2.1

Item {
    id: item_pageimg
    width: 320
    height: 480
    property alias button_back: button_back
    property alias button_ok_img: button_ok_img
    property alias button_cancel_img: button_cancel_img
    property alias listView_img: listView_img
    property alias listModel_img: listModel_img
    property int selectedIndex
    onSelectedIndexChanged: listView_img.currentIndex = selectedIndex

    ColumnLayout {
        id: columnLayout_pageimg
        anchors.fill: parent

        ListView {
            id: listView_img
            width: parent.width
            height: 360
            orientation: ListView.Vertical
            Layout.fillHeight: true
            Layout.fillWidth: true

            model: FolderListModel {
                id: listModel_img
                nameFilters: ["*.jpg", "*.png", "*.gif", "*.bmp"]
                showDirsFirst: true
            }

            delegate: Item {
                id: item_listViewItem
                width: parent.width
                height: 20
                Text {
                    text: fileName
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 0
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
            id: rowLayout_pageimg
            width: parent.width
            height: 100
            Layout.fillHeight: true
            Layout.fillWidth: true

            Button {
                id: button_back
                height: parent.height
                text: qsTr("Back")
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5

                onClicked: listModel_img.folder = listModel_img.parentFolder
            }

            Button {
                id: button_ok_img
                height: parent.height
                text: qsTr("OK")
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5
            }

            Button {
                id: button_cancel_img
                height: parent.height
                text: qsTr("Cancel")
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5
            }
        }
    }
}
