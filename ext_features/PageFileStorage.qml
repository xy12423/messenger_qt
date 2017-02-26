import QtQuick 2.7

PageFileStorageForm {
    selectedIndex: 0

    signal reqFinish()

    Connections {
        target: cppInterface
        onRefreshFilelist: {
            listModel_fstorage.clear()
            for (var i = 0; i < files.length; i++)
                listModel_fstorage.append({ name: files[i] })
        }
    }

    button_download.onClicked: {
        if (selectedIndex != 0)
            cppInterface.reqDownloadFile(selectedIndex)
        reqFinish()
    }

    button_cancel.onClicked: {
        reqFinish()
    }

    Component.onCompleted: {
        cppInterface.reqFilelist()
    }
}
