import QtQuick 2.7

PageFileForm {
    id: form_pagefile

    signal imageReqFinish()

    button_back.onClicked: {
        listModel_file.folder = listModel_file.parentFolder
        text_path.text = cppInterface.urlToLocalStr(listModel_file.folder)
    }

    onSelectedIndexChanged: {
        var selectedURL = listModel_file.get(selectedIndex, "fileURL")
        if (listModel_file.get(selectedIndex, "fileIsDir"))
        {
            listModel_file.folder = selectedURL
            text_path.text = cppInterface.urlToLocalStr(selectedURL)
            listView_file.currentIndex = -1
            selectedIndex = -1
        }
    }

    button_ok_file.onClicked: {
        var selectedURL = listModel_file.get(selectedIndex, "fileURL")
        if (!listModel_file.get(selectedIndex, "fileIsDir"))
            cppInterface.sendImg(selectedURL)
        imageReqFinish()
    }

    button_cancel_file.onClicked: {
        imageReqFinish()
    }

    Component.onCompleted: {
        listModel_file.nameFilters = ["*.jpg", "*.png", "*.gif", "*.bmp"]
        listModel_file.folder = cppInterface.getPicturesPath()
        text_path.text = cppInterface.urlToLocalStr(listModel_file.folder)
    }
}
