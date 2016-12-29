import QtQuick 2.7

PageFileForm {
    id: form_pagefile

    signal fileReqFinish()

    button_back.onClicked: {
        listModel_file.folder = listModel_file.parentFolder
        selectedIndex = -1
        text_path.text = cppInterface.urlToLocalStr(listModel_file.folder)
    }

    onSelectedIndexChanged: {
        var selectedURL = listModel_file.get(selectedIndex, "fileURL")
        if (listModel_file.get(selectedIndex, "fileIsDir"))
        {
            listModel_file.folder = selectedURL
            selectedIndex = -1
            text_path.text = cppInterface.urlToLocalStr(selectedURL)
        }
    }

    button_ok_file.onClicked: {
        var selectedURL = listModel_file.get(selectedIndex, "fileURL")
        if (!listModel_file.get(selectedIndex, "fileIsDir"))
            cppInterface.sendFile(selectedURL)
        fileReqFinish()
    }

    button_cancel_file.onClicked: {
        fileReqFinish()
    }

    Component.onCompleted: {
        listModel_file.nameFilters = ["*.*"]
        listModel_file.folder = cppInterface.getDownloadPath()
        selectedIndex = -1
        text_path.text = cppInterface.urlToLocalStr(listModel_file.folder)
    }
}
