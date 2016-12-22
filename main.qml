import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

ApplicationWindow {
    visible: true
    width: 320
    height: 480
    title: qsTr("messenger")

    StackView {
        id: stack_view
        initialItem: main_page
        anchors.fill: parent
    }

    Component {
        id: component_page_join

        Item {
            PageJoin {
                id: page_join
                anchors.fill: parent

                Connections {
                    target: page_join;
                    onConnectReqFinish: {
                        stack_view.pop()
                    }
                }
            }
        }
    }

    Page {
        id: main_page

        SwipeView {
            id: swipeView
            anchors.fill: parent
            currentIndex: tabBar.currentIndex

            PageList {
                id: page_list
                Connections {
                    target: page_list;
                    onConnectReq: {
                        stack_view.push(component_page_join)
                    }
                }
            }

            PageChat {
            }
        }

        header: TabBar {
            id: tabBar
            currentIndex: swipeView.currentIndex
            TabButton {
                text: qsTr("User list")
            }
            TabButton {
                text: qsTr("Chat")
            }
        }
    }
}
