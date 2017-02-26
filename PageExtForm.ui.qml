import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: item_pageext
    width: 320
    height: 480
    property alias listView_exts: listView_exts
    property alias listModel_exts: listModel_exts
    property int selection
    property int watcher

    ListView {
        id: listView_exts
        anchors.fill: parent
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
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 0
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: selection = feature_id, watcher = 1 - watcher
            }
        }
    }
}
