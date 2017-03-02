import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

Item {
    width: 320
    height: 480
    property alias text_content: text_content
    property alias text_input: text_input
    property alias button_send: button_send
    property alias button_file: button_file
    property alias button_image: button_image

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Item
        {
            width: parent.width
            height: 405
            Layout.fillHeight: true
            Layout.fillWidth: true

            Flickable {
                anchors.fill: parent
                flickableDirection: Flickable.VerticalFlick

                TextArea.flickable: TextArea {
                    id: text_content

                    textFormat: Text.RichText
                    font.pixelSize: 15
                    textMargin: 2
                    wrapMode: TextArea.Wrap
                    readOnly: true
                }

                ScrollBar.vertical: ScrollBar {}
                ScrollBar.horizontal: ScrollBar {}
            }
        }

        Rectangle {
            width: parent.width
            height: 35
            Layout.fillHeight: true
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
            width: parent.width
            height: 40
            Layout.fillHeight: true
            Layout.fillWidth: true

            Button {
                id: button_send
                Layout.fillHeight: true
                Layout.fillWidth: true

                text: qsTr("Send")
            }

            Button {
                id: button_image
                Layout.fillHeight: true
                Layout.fillWidth: true

                text: qsTr("Image")
            }

            Button {
                id: button_file
                Layout.fillHeight: true
                Layout.fillWidth: true

                text: qsTr("File")
            }
        }
    }
}
