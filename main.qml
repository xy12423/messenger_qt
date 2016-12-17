import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

ApplicationWindow {
    visible: true
    width: 320
    height: 480
    title: qsTr("messenger")

    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        PageJoin {
        }

        PageList {
        }

        PageChat {
        }
    }

    header: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex
        TabButton {
            text: qsTr("Connect to")
        }
        TabButton {
            text: qsTr("User list")
        }
        TabButton {
            text: qsTr("Chat")
        }
    }
}
