import QtQuick 2.8

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

    Image {
        width: cppInterface.windowWidth
        fillMode: Image.PreserveAspectFit
        source: cppInterface.localStrToUrl(content)
        sourceSize.width: 1024
    }
}
