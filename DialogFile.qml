import QtQuick 2.7
import QtQuick.Dialogs 1.0

FileDialog {
    id: fileDialog
    title: "Open file"
    folder: shortcuts.home
    onAccepted: {
        //cppInterface.sendFile(fileDialog.fileUrls)
        fileDialog.close()
    }
    onRejected: {
        fileDialog.close()
    }
    Component.onCompleted: visible = true
}
