import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

Item {
    width: 320
    height: 480
    property alias listView_users: listView_users
    property alias listModel_users: listModel_users
    property alias button_add: button_add
    property alias button_del: button_del

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        ListView {
            id: listView_users
            width: parent.width
            Layout.preferredHeight: 440
            Layout.fillHeight: true
            Layout.fillWidth: true

            clip: true
            orientation: ListView.Vertical
            currentIndex: cppInterface.index

            model: ListModel {
                id: listModel_users
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
                    onClicked: cppInterface.index = index
                }
            }
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        }

        RowLayout {
            width: parent.width
            Layout.preferredHeight: 40
            Layout.fillHeight: true
            Layout.fillWidth: true

            Button {
                id: button_add
                text: qsTr("Connect")
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Button {
                id: button_del
                text: qsTr("Disconnect")
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
