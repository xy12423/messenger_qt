import QtQuick 2.8

PageExtForm {
    signal selectFileStorage()
    signal selectKeyMan()

    Connections {
        target: cppInterface
        onEnableFeature: {
            listModel_exts.clear()
            listModel_exts.append({ name: "Key management", feature_id: 2 })
            if (flag % 2 == 1)
                listModel_exts.append({ name: "File storage", feature_id: 1 })
        }
    }

    onWatcherChanged: {
        if (selection == 1)
            selectFileStorage()
        else if (selection == 2)
            selectKeyMan()
    }

    Component.onCompleted: {
        listModel_exts.append({ name: "Key management", feature_id: 2 })
    }
}
