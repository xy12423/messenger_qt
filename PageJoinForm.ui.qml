import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

Item {
    width: 320
    height: 480
    property alias button_cancel_conn: button_cancel_conn
    property alias button_conn: button_conn
    property alias textInput_addr: textInput_addr
    property alias textInput_port: textInput_port
    property alias listView_join: listView_join
    property alias listModel_join: listModel_join
    property int reqConnWatcher: 0
    property int reqConnIndex: -1
    property int reqConnDelWatcher: 0
    property int reqConnDelIndex: -1

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            width: parent.width
            Layout.preferredHeight: 24
            Layout.maximumHeight: 24
            Layout.fillHeight: true
            Layout.fillWidth: true

            spacing: 5

            Text {
                height: parent.height
                Layout.fillHeight: true
                leftPadding: 5

                verticalAlignment: Text.AlignVCenter
                text: qsTr("Address:")
                font.pixelSize: 12
            }

            Rectangle {
                height: parent.height
                Layout.fillHeight: true
                Layout.fillWidth: true

                color: "#cccccc"

                TextInput {
                    id: textInput_addr
                    anchors.fill: parent

                    leftPadding: 5
                    rightPadding: 5
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 12
                }
            }

            Text {
                height: parent.height
                Layout.fillHeight: true

                verticalAlignment: Text.AlignVCenter
                text: qsTr("Port:")
                font.pixelSize: 12
            }

            Rectangle {
                height: parent.height
                Layout.preferredWidth: 40
                Layout.fillHeight: true

                color: "#cccccc"

                TextInput {
                    id: textInput_port
                    anchors.fill: parent

                    leftPadding: 5
                    rightPadding: 5
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: 12

                    text: "4826"
                }
            }
        }

        ListView {
            id: listView_join
            width: parent.width
            Layout.preferredHeight: 400
            Layout.fillHeight: true
            Layout.fillWidth: true

            clip: true
            orientation: ListView.Vertical
            spacing: 5

            model: ListModel {
                id: listModel_join
            }

            delegate: RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 20

                Item {
                    Layout.fillHeight: true
                    Layout.fillWidth: true

                    Text {
                        anchors.fill: parent
                        text: address + ":" + portStr

                        leftPadding: 5
                        rightPadding: 5
                        verticalAlignment: Text.AlignVCenter
                    }

                    MouseArea {
                        anchors.fill: parent

                        onPressed: reqConnIndex = index, reqConnWatcher = 1 - reqConnWatcher
                    }
                }

                Button {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    Layout.maximumWidth: parent.height
                    Layout.preferredWidth: parent.height

                    text: "×"

                    onPressed: reqConnDelIndex = index, reqConnDelWatcher = 1 - reqConnDelWatcher
                }
            }
        }

        RowLayout {
            width: parent.width
            Layout.preferredHeight: 40
            Layout.maximumHeight: 60
            Layout.fillHeight: true
            Layout.fillWidth: true

            Button {
                id: button_conn
                height: parent.height
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: qsTr("Connect")
            }

            Button {
                id: button_cancel_conn
                height: parent.height
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: qsTr("Cancel")
            }
        }
    }
}
