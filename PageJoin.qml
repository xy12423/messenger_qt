import QtQuick 2.7

PageJoinForm {
    Connections {
        target: button_add;
        onClicked: {
            cppInterface.connectTo(textInput_addr.text, textInput_port.text)
        }
    }
}
