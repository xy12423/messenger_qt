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
    property alias text_path: text_path
    property int selectedIndex
    onSelectedIndexChanged: listView_img.currentIndex = selectedIndex

    ColumnLayout {
        id: columnLayout_pageimg
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
            id: listView_img
            width: parent.width
            height: 360
            orientation: ListView.Vertical
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredHeight: 360

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
            id: rowLayout_footer
            width: parent.width
            height: 40
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.preferredHeight: 40

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
