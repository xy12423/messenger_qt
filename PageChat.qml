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
            listModel_chat.append({ "id": id, "compType": "ItemChatText.qml", "from": from, "content": message })
            if (scrollDown)
                listView_chat.positionViewAtEnd()
        }
    }

    Connections {
        target: cppInterface
        onChatImage: {
            var scrollDown = listView_chat.atYEnd
            listModel_chat.append({ "id": id, "compType": "ItemChatImage.qml", "from": from, "content": imgPath })
            if (scrollDown)
                listView_chat.positionViewAtEnd()
        }
    }

    Connections {
        target: cppInterface
        onChatFile: {
            var scrollDown = listView_chat.atYEnd
            listModel_chat.append({ "id": id, "compType": "ItemChatFile.qml", "from": from, "content": filename })
            if (scrollDown)
                listView_chat.positionViewAtEnd()
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
