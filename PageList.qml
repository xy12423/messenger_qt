import QtQuick 2.8

PageListForm {
    id: form_pagelist
    signal connectReq()

    Connections {
        target: cppInterface
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
        target: cppInterface
        onLeft: {
            listModel_users.remove(index)
        }
    }

    button_add.onClicked: {
        form_pagelist.connectReq()
    }

    button_del.onClicked: {
        cppInterface.disconnect()
    }
}
