import QtQuick 2.7

PageExtForm {
    signal selectFileStorage()
    watcher: 1

    Connections {
        target: cppInterface
        onEnableFeature: {
            listModel_exts.clear()
            if (flag % 2 == 1)
                listModel_exts.append({ name: "File storage", feature_id: 1 })
        }
    }

    onWatcherChanged: {
        if (selection == 1)
            selectFileStorage()
    }
}
