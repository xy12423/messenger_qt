import QtQuick 2.7
import QtQuick.Dialogs 1.0

FileDialog {
    id: imgDialog
    title: "Open image"
    folder: shortcuts.pictures
    onAccepted: {
        cppInterface.sendImg(imgDialog.fileUrl)
        imgDialog.close()
    }
    onRejected: {
        imgDialog.close()
    }
    Component.onCompleted: visible = true
}
