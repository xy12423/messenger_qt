import QtQuick 2.8

PageJoinForm {
    id: form_pagejoin

    signal connectReqFinish()

    Connections {
        target: cppInterface
        onRefreshConnHistory: {
            listModel_join.clear()
            for (var i = 0; i < addr.length; i++)
                listModel_join.append({ address: addr[i], portStr: port[i] })
        }
    }

    onReqConnWatcherChanged: {
        var item = listModel_join.get(reqConnIndex)
        textInput_addr.text = item.address
        textInput_port.text = item.portStr
    }

    onReqConnDelWatcherChanged: {
        cppInterface.reqConnHistoryDel(reqConnDelIndex)
    }

    button_conn.onClicked: {
        cppInterface.connectTo(textInput_addr.text, textInput_port.text)
        form_pagejoin.connectReqFinish()
    }

    button_clrhis.onClicked: {
        cppInterface.reqConnHistoryClr()
    }

    button_cancel_conn.onClicked: {
        form_pagejoin.connectReqFinish()
    }

    Component.onCompleted: {
        cppInterface.reqConnHistory()
    }
}
