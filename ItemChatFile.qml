import QtQuick 2.8
import QtQuick.Controls 2.1

Column {
    spacing: 5

    Text {
        width: cppInterface.windowWidth
        leftPadding: 5
        rightPadding: 5

        text: from
        font.bold: true
        font.pixelSize: 12
    }

    Text {
        width: cppInterface.windowWidth
        leftPadding: 5
        rightPadding: 5

        text: content
        font.pixelSize: 12
    }

    ProgressBar {
        id: progress_itemChatFile
        width: cppInterface.windowWidth
        height: 15
        to: 100
    }

    Connections {
        target: cppInterface

        onNotifyFileProgress: {
            if (fid == id)
                progress_itemChatFile.value = progress
        }
    }
}
