import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

Item {
    width: 320
    height: 480
    property alias listView_fstorage: listView_fstorage
    property alias listModel_fstorage: listModel_fstorage
    property alias button_download: button_download
    property alias button_cancel: button_cancel
    property int selectedIndex: -1

    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: listView_fstorage
            width: parent.width
            height: 440
            Layout.fillHeight: true
            Layout.fillWidth: true

            clip: true
            orientation: ListView.Vertical
            currentIndex: selectedIndex

            model: ListModel {
                id: listModel_fstorage
            }

            delegate: Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 20
                Text {
                    text: name
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 0
                    leftPadding: 5
                    rightPadding: 5
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: selectedIndex = index
                }
            }
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        }

        RowLayout {
            width: parent.width
            height: 40
            Layout.fillWidth: true

            Button {
                id: button_download
                height: parent.height
                text: qsTr("Download")
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Button {
                id: button_cancel
                height: parent.height
                text: qsTr("Cancel")
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
