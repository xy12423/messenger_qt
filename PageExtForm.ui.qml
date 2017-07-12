import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

Item {
    width: 320
    height: 480
    property alias listView_exts: listView_exts
    property alias listModel_exts: listModel_exts
    property int selection: -1
    property int watcher: 1

    ListView {
        id: listView_exts
        anchors.fill: parent
        clip: true
        model: ListModel {
            id: listModel_exts
        }
        delegate: Item {
            anchors.left: parent.left
            anchors.right: parent.right
            height: 40
            Rectangle {
                anchors.left: parent.left
                anchors.leftMargin: 5
                anchors.right: parent.right
                anchors.rightMargin: 5
                anchors.top: parent.top
                anchors.topMargin: 5
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 5

                color: "lightsteelblue"
                radius: 5

                Text {
                    text: name
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 0
                    leftPadding: 5
                    rightPadding: 5
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: selection = feature_id, watcher = 1 - watcher
            }
        }
    }
}
