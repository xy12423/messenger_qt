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
            height: 405
            Layout.fillHeight: true
            Layout.fillWidth: true

            Flickable {
                id: flickable_chat
                anchors.fill: parent
                flickableDirection: Flickable.VerticalFlick

                TextArea.flickable: TextArea {
                    id: text_content

                    textFormat: Text.RichText
                    font.pointSize: 15
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
            height: 35
            Layout.fillWidth: true

            color: "#e6e6e6"

            TextField {
                id: text_input
                anchors.fill: parent
                bottomPadding: 5
                rightPadding: 5
                leftPadding: 5
                topPadding: 5
                verticalAlignment: TextField.AlignVCenter
            }
        }

        RowLayout {
            id: rowLayout_pagechat
            width: parent.width
            height: 40
            Layout.fillWidth: true

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
