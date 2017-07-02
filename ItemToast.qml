import QtQuick 2.8
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3

Rectangle {
    id: rect_toast
    width: childrenRect.width
    height: childrenRect.height
    color: "#666666"
    opacity: 0
    radius: height / 4

    Behavior on opacity {
        NumberAnimation { duration: 500 }
    }

    ColumnLayout {
        Label {
            id: label_toast
            Layout.margins: 5
            color: "white"
        }
    }

    Timer {
        id: timer_toast
        interval: 1000
        onTriggered: rect_toast.opacity = 0
    }

    function showToast(text) {
        label_toast.text = text
        rect_toast.opacity = 1
        timer_toast.restart()
    }
}
