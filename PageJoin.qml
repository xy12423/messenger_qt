import QtQuick 2.7

PageJoinForm {
    id: form_pagejoin

    signal connectReqFinish()

    button_conn.onClicked: {
        cppInterface.connectTo(textInput_addr.text, textInput_port.text)
        form_pagejoin.connectReqFinish()
    }

    button_cancel_conn.onClicked: {
        form_pagejoin.connectReqFinish()
    }
}
