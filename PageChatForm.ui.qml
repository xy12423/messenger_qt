import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    id: item_pagechat
    width: 320
    height: 480
    property alias text_content: text_content
    property alias text_input: text_input
    property alias button_send: button_send
    property alias button_file: button_file
    property alias button_image: button_image

    ColumnLayout {
        id: columnLayout_pagechat
        spacing: 0
        anchors.fill: parent

        Item
        {
            width: parent.width
            height: 360
            Layout.minimumHeight: 320
            Layout.fillHeight: true
            Layout.fillWidth: true

            Flickable {
                id: flickable_chat
                anchors.fill: parent
                flickableDirection: Flickable.VerticalFlick

                TextArea.flickable: TextArea {
                    id: text_content

                    textFormat: Text.RichText
                    font.pixelSize: 12
                    textMargin: 2
                    wrapMode: TextArea.Wrap
                    readOnly: true
                }

                ScrollBar.vertical: ScrollBar {}
            }
        }

        Rectangle {
            id: rectangle_input
            width: parent.width
            height: 20
            Layout.minimumHeight: 20
            Layout.preferredHeight: 20
            Layout.fillHeight: true
            Layout.fillWidth: true

            color: "#e6e6e6"

            TextInput {
                id: text_input
                anchors.fill: parent

                font.pixelSize: 12
            }
        }

        Item {
            Layout.minimumHeight: 5
            Layout.preferredHeight: 5
        }

        RowLayout {
            id: rowLayout_pagechat
            width: parent.width
            height: 50
            Layout.minimumHeight: 50
            Layout.preferredHeight: 50

            Button {
                id: button_send
                height: parent.height
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5

                text: qsTr("Send")
            }

            Button {
                id: button_image
                height: parent.height
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5

                text: qsTr("Image")
            }

            Button {
                id: button_file
                height: parent.height
                Layout.fillHeight: true
                Layout.fillWidth: true
                spacing: 5

                text: qsTr("File")
            }
        }
    }
}
