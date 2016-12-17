import QtQuick 2.7

PageListForm {
    Connections {
        target: cppInterface;
        onJoined: {
            listModel_users.insert(index, { "name": name })
        }
    }

    Connections {
        target: cppInterface;
        onSelectIndex: {
            listView_users.currentIndex = index
        }
    }

    Connections {
        target: cppInterface;
        onLeft: {
            listModel_users.remove(index)
        }
    }

    Connections {
        target: button_del;
        onClicked: {
            cppInterface.disconnect()
        }
    }
}
