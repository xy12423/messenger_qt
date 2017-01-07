import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: item_pagejoin
    width: 320
    height: 480
    property alias button_cancel_conn: button_cancel_conn
    property alias button_conn: button_conn
    property alias textInput_addr: textInput_addr
    property alias textInput_port: textInput_port

    RowLayout {
        id: rowLayout_addr
        width: 200
        height: 20
        anchors.verticalCenterOffset: -60
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter


        Text {
            id: text_addr
            height: 20
            Layout.fillHeight: true
            text: qsTr("Address:")
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 12
        }

        Rectangle {
            id: rectangle_addr
            height: 20
            Layout.fillHeight: true
            Layout.fillWidth: true
            anchors.left: text_addr.right
            anchors.leftMargin: 5

            color: "#cccccc"

            TextInput {
                id: textInput_addr
                anchors.fill: parent

                verticalAlignment: Text.AlignVCenter
                font.pointSize: 12
            }
        }
    }

    RowLayout {
        id: rowLayout_port
        width: 200
        height: 20
        anchors.top: rowLayout_addr.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            id: text_port
            height: 20
            Layout.fillHeight: true
            text: qsTr("Port:")
            verticalAlignment: Text.AlignVCenter
            font.pointSize: 12
        }

        Rectangle {
            id: rectangle_port
            height: 20
            Layout.fillHeight: true
            Layout.fillWidth: true
            anchors.left: text_port.right
            anchors.leftMargin: 5

            color: "#cccccc"

            TextInput {
                id: textInput_port
                anchors.fill: parent

                text: "4826"
                verticalAlignment: Text.AlignVCenter
                font.pointSize: 12
            }
        }
    }

    Button {
        id: button_conn
        width: 150
        height: 18
        anchors.top: rowLayout_port.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Connect")
    }

    Button {
        id: button_cancel_conn
        width: 150
        height: 18
        anchors.top: button_conn.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Cancel")
    }
}
