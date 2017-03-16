import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

Item {
    width: 320
    height: 480
    property alias listView_keyman: listView_keyman
    property alias listModel_keyman: listModel_keyman
    property alias text_keyex: text_keyex
    property alias button_trust: button_trust
    property alias button_distrust: button_distrust
    property alias button_import: button_import
    property alias button_export: button_export
    property alias button_save: button_save
    property alias button_back: button_back
    property int selectedIndex: -1

    ColumnLayout {
        anchors.fill: parent

        ListView {
            id: listView_keyman
            width: parent.width
            height: 360
            orientation: ListView.Vertical
            Layout.fillHeight: true
            Layout.fillWidth: true
            currentIndex: selectedIndex

            model: ListModel {
                id: listModel_keyman
            }

            delegate: Item {
                anchors.left: parent.left
                anchors.right: parent.right
                height: 20
                Text {
                    text: name
                    anchors.left: parent.left
                    anchors.leftMargin: 5
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: 0
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: selectedIndex = index
                }
            }
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        }

        Rectangle {
            width: parent.width
            height: 50
            Layout.fillHeight: true
            Layout.fillWidth: true

            color: "#e6e6e6"

            TextArea {
                id: text_keyex
                anchors.fill: parent
                bottomPadding: 5
                rightPadding: 5
                leftPadding: 5
                topPadding: 5
                verticalAlignment: TextField.AlignVCenter
            }
        }

        GridLayout {
            width: parent.width
            height: 70
            Layout.fillHeight: true
            Layout.fillWidth: true
            columns: 2

            Button {
                id: button_trust
                text: qsTr("Trust")
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Button {
                id: button_distrust
                text: qsTr("Distrust")
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Button {
                id: button_import
                text: qsTr("Import")
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Button {
                id: button_export
                text: qsTr("Export")
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Button {
                id: button_save
                text: qsTr("Save")
                Layout.fillHeight: true
                Layout.fillWidth: true
            }

            Button {
                id: button_back
                text: qsTr("Back")
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
        }
    }
}
