import QtQuick 2.8

MouseArea
{
    width: childrenRect.width
    height: childrenRect.height

    Column {
        spacing: 5

        Text {
            width: cppInterface.windowWidth
            leftPadding: 5
            rightPadding: 5

            text: from
            font.bold: true
            font.pixelSize: 12
            wrapMode: Text.Wrap
        }

        Text {
            width: cppInterface.windowWidth
            leftPadding: 7
            rightPadding: 5

            text: content
            font.pixelSize: 12
            wrapMode: Text.Wrap
        }
    }

    onPressAndHold: {
        cppInterface.setClipboard(content)
        cppInterface.print(qsTr("Text copied"))
    }
}
