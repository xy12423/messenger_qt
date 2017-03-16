import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import "ext_features"

ApplicationWindow {
    id: main_window
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
                anchors.fill: parent

                onConnectReqFinish: {
                    stack_view.pop()
                }
            }
        }
    }

    Component {
        id: component_page_img

        Item {
            PageFile {
                anchors.fill: parent

                fileIcon: "qrc:/images/image.png"
                startFolder: cppInterface.getPicturesPath()
                filter: ["*.jpg", "*.png", "*.gif", "*.bmp"]

                onReqFinishOK: {
                    cppInterface.sendImg(path)
                    stack_view.pop()
                }

                onReqFinishCancel: {
                    stack_view.pop()
                }
            }
        }
    }

    Component {
        id: component_page_file

        Item {
            PageFile {
                anchors.fill: parent
                startFolder: cppInterface.getDownloadPath()

                onReqFinishOK: {
                    cppInterface.sendFile(path)
                    stack_view.pop()
                }

                onReqFinishCancel: {
                    stack_view.pop()
                }
            }
        }
    }

    Component {
        id: component_page_impkey

        Item {
            PageFile {
                anchors.fill: parent
                startFolder: cppInterface.getDownloadPath()

                onReqFinishOK: {
                    cppInterface.importKey(path)
                    stack_view.pop()
                }

                onReqFinishCancel: {
                    stack_view.pop()
                }
            }
        }
    }

    Component {
        id: component_page_expkey

        Item {
            id: item_page_expkey
            property alias keyIndex: page_expkey.keyIndex

            PageExpKey {
                id: page_expkey
                anchors.fill: parent

                onReqFinish: {
                    stack_view.pop()
                }
            }
        }
    }

    Component {
        id: component_page_fstorage

        Item {
            PageFileStorage {
                anchors.fill: parent

                onReqFinish: {
                    stack_view.pop()
                }
            }
        }
    }

    Component {
        id: component_page_keyman

        Item {
            PageKeyMan {
                anchors.fill: parent

                onReqImport: {
                    stack_view.push(component_page_impkey)
                }

                onReqExport: {
                    stack_view.push(component_page_expkey, { "keyIndex": keyID })
                }

                onReqFinish: {
                    stack_view.pop()
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
                onConnectReq: {
                    stack_view.push(component_page_join)
                }
            }

            PageChat {
                onImageReq: {
                    if (cppInterface.index != -1)
                        stack_view.push(component_page_img)
                }

                onFileReq: {
                    if (cppInterface.index != -1)
                        stack_view.push(component_page_file)
                }
            }

            PageExt {
                onSelectKeyMan: {
                    stack_view.push(component_page_keyman)
                }

                onSelectFileStorage: {
                    if (cppInterface.index != -1)
                        stack_view.push(component_page_fstorage)
                }
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
            TabButton {
                text: qsTr("Extra")
            }
        }
    }

    Timer {
        id: timer_close
        interval: 320
    }

    onClosing: {
        if (Qt.platform.os == "android")
        {
            if (stack_view.depth > 1)
            {
                close.accepted = false;
                stack_view.pop();
            }
            else if (!timer_close.running)
            {
                close.accepted = false;
                timer_close.start()
            }
        }
    }
}
