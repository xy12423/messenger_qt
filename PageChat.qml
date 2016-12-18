import QtQuick 2.7

PageChatForm {
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
}
