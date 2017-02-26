import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: item_pagefstorage
    width: 320
    height: 480
    property alias listView_fstorage: listView_fstorage
    property alias listModel_fstorage: listModel_fstorage
    property alias button_download: button_download
    property alias button_cancel: button_cancel
    property int selectedIndex

    ColumnLayout {
        id: columnLayout_pagefstorage
        anchors.fill: parent

        ListView {
            id: listView_fstorage
            width: parent.width
            height: 440
            orientation: ListView.Vertical
            Layout.fillHeight: true
            Layout.fillWidth: true
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
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 0
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: selectedIndex = index
                }
            }
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            focus: true
        }

        RowLayout {
            id: rowLayout_pagefstorage
            width: parent.width
            height: 40
            Layout.fillWidth: true

            Button {
                id: button_download
                height: parent.height
                text: qsTr("Download")
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5
            }

            Button {
                id: button_cancel
                height: parent.height
                text: qsTr("Cancel")
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5
            }
        }
    }
}
