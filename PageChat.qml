import QtQuick 2.7

PageChatForm {
    id: form_pagechat
    signal imageReq()
    signal fileReq()

    Connections {
        target: cppInterface;
        onRefreshChat: {
            text_content.clear()
            text_content.append(content)
        }
    }

    Connections {
        target: button_send;
        onClicked: {
            cppInterface.sendMsg(text_input.text)
            text_input.text = ""
        }
    }

    Connections {
        target: button_image;
        onClicked: {
            form_pagechat.imageReq()
        }
    }

    Connections {
        target: button_file;
        onClicked: {
            form_pagechat.fileReq()
        }
    }
}
