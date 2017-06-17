import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

Item {
    width: 320
    height: 480
    property alias listView_chat: listView_chat
    property alias listModel_chat: listModel_chat
    property alias text_input: text_input
    property alias button_send: button_send
    property alias button_file: button_file
    property alias button_image: button_image
    property int textEnterWatcher: 1

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        ListView {
            id: listView_chat
            width: parent.width
            Layout.preferredHeight: 405
            Layout.fillHeight: true
            Layout.fillWidth: true
            topMargin: 5
            bottomMargin: 5

            clip: true
            orientation: ListView.Vertical
            spacing: 5

            model: ListModel {
                id: listModel_chat
            }

            delegate: Item {
                height: loader_chat.height
                Loader {
                    id: loader_chat
                    source: compType
                }
            }
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        }

        Rectangle {
            width: parent.width
            Layout.preferredHeight: 35
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

                Keys.onReturnPressed: textEnterWatcher = textEnterWatcher - 1
                Keys.onEnterPressed: textEnterWatcher = textEnterWatcher - 1
            }
        }

        RowLayout {
            width: parent.width
            Layout.preferredHeight: 40
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
