import QtQuick 2.8

PageImgForm {
    property url startFolder: cppInterface.getPicturesPath()
    property var filter: ["*.jpg", "*.png", "*.gif", "*.bmp"]

    signal reqFinishOK(url path)
    signal reqFinishCancel()

    button_back.onClicked: {
        if (listModel_file.parentFolder === cppInterface.localStrToUrl(""))
            return
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
        if (!listModel_file.isFolder(selectedIndex))
            reqFinishOK(listModel_file.get(selectedIndex, "fileURL"))
        else
            reqFinishCancel()
    }

    button_cancel_file.onClicked: {
        reqFinishCancel()
    }

    Component.onCompleted: {
        listModel_file.nameFilters = filter
        listModel_file.folder = startFolder
        text_path.text = cppInterface.urlToLocalStr(listModel_file.folder)
    }
}
