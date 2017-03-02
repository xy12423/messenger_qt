import QtQuick 2.8
import ".."

PageFileForm {
    folderIcon: "qrc:/images/folder.png"
    fileIcon: "qrc:/images/file.png"
    property int keyIndex: -1

    signal reqFinish()

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
        if (selectedIndex < 1)
            cppInterface.exportKey(keyIndex, listModel_file.folder, "publickey")
        else
        {
            if (listModel_file.isFolder(selectedIndex))
                cppInterface.exportKey(keyIndex, listModel_file.get(selectedIndex, "fileURL"), "publickey")
            else
                cppInterface.exportKey(keyIndex, listModel_file.folder, listModel_file.get(selectedIndex, "fileName"))
        }
        reqFinish()
    }

    button_cancel_file.onClicked: {
        reqFinish()
    }

    Component.onCompleted: {
        listModel_file.nameFilters = ["*"]
        listModel_file.folder = cppInterface.getDownloadPath()
        text_path.text = cppInterface.urlToLocalStr(listModel_file.folder)
    }
}
