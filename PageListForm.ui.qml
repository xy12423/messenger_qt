import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: item_pagelist
    width: 320
    height: 480
    property alias listView_users: listView_users
    property alias listModel_users: listModel_users
    property alias button_add: button_add
    property alias button_del: button_del

    ColumnLayout {
        id: columnLayout_pagelist
        anchors.fill: parent

        ListView {
            id: listView_users
            width: parent.width
            height: 360
            orientation: ListView.Vertical
            Layout.fillHeight: true
            Layout.fillWidth: true

            model: ListModel {
                id: listModel_users
            }

            delegate: Item {
                id: item_listViewItem
                width: parent.width
                height: 20
                Text {
                    text: name
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 0
                }
                MouseArea {
                    id: mouse_listViewItem

                    anchors.fill: parent
                    onClicked: cppInterface.index = index
                }
            }
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            focus: true
        }

        RowLayout {
            id: rowLayout_pagelist
            width: parent.width
            height: 40
            Layout.fillWidth: true

            Button {
                id: button_add
                height: parent.height
                text: qsTr("Connect")
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5
            }

            Button {
                id: button_del
                height: parent.height
                text: qsTr("Disconnect")
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5
            }
        }
    }
}
