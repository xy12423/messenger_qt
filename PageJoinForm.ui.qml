import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: item_pagejoin
    width: 320
    height: 480
    property alias textInput_addr: textInput_addr
    property alias textInput_port: textInput_port
    property alias button_add: button_add

    RowLayout {
        id: rowLayout_addr
        width: 150
        height: 18
        anchors.verticalCenterOffset: -50
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter


        Text {
            id: text_addr
            width: 55
            height: 18
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: qsTr("Address:")
            font.pixelSize: 12
        }

        Rectangle {
            id: rectangle_addr
            width: 90
            height: 18
            Layout.minimumWidth: 90
            Layout.fillHeight: true
            Layout.fillWidth: true
            anchors.left: text_addr.right
            anchors.leftMargin: 5

            color: "#cccccc"

            TextInput {
                id: textInput_addr
                anchors.fill: parent

                font.pixelSize: 12
            }
        }
    }

    RowLayout {
        id: rowLayout_port
        width: 150
        height: 18
        anchors.top: rowLayout_addr.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter

        Text {
            id: text_port
            width: 30
            height: 18
            Layout.fillHeight: true
            Layout.fillWidth: true
            text: qsTr("Port:")
            font.pixelSize: 12
        }

        Rectangle {
            id: rectangle_port
            width: 100
            height: 18
            Layout.minimumWidth: 100
            Layout.fillHeight: true
            Layout.fillWidth: true
            anchors.left: text_port.right
            anchors.leftMargin: 5

            color: "#cccccc"

            TextInput {
                id: textInput_port
                anchors.fill: parent

                font.pixelSize: 12
            }
        }
    }

    Button {
        id: button_add
        width: 150
        height: 18
        anchors.top: rowLayout_port.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        text: qsTr("Connect")
    }
}
