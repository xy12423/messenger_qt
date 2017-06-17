import QtQuick 2.8

PageChatForm {
    id: form_pagechat
    signal imageReq()
    signal fileReq()

    Connections {
        target: cppInterface
        onChatClear: {
            listModel_chat.clear()
        }
    }

    Connections {
        target: cppInterface
        onChatText: {
            var scrollDown = listView_chat.atYEnd
            listModel_chat.append({ "compType": "ItemChatText.qml", "from": from, "content": message })
            if (scrollDown)
            {
                listView_chat.positionViewAtEnd()
                listView_chat.currentIndex = listView_chat.count - 1
            }
        }
    }

    Connections {
        target: cppInterface
        onChatImage: {
            var scrollDown = listView_chat.atYEnd
            listModel_chat.append({ "compType": "ItemChatImage.qml", "from": from, "content": imgPath })
            if (scrollDown)
            {
                listView_chat.positionViewAtEnd()
                listView_chat.currentIndex = listView_chat.count - 1
            }
        }
    }

    onTextEnterWatcherChanged: {
        cppInterface.sendMsg(text_input.text)
        text_input.text = ""
    }

    button_send.onClicked: {
        cppInterface.sendMsg(text_input.text)
        text_input.text = ""
    }

    button_image.onClicked: {
        form_pagechat.imageReq()
    }

    button_file.onClicked: {
        form_pagechat.fileReq()
    }
}
