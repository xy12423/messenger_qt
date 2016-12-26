import QtQuick 2.7

PageImgForm {
    id: form_pageimg

    signal imageReqFinish()

    button_back.onClicked: {
        listModel_img.folder = listModel_img.parentFolder
        text_path.text = cppInterface.urlToLocalStr(listModel_img.folder)
    }

    onSelectedIndexChanged: {
        var selectedURL = listModel_img.get(selectedIndex, "fileURL")
        if (listModel_img.get(selectedIndex, "fileIsDir"))
        {
            listModel_img.folder = selectedURL
            text_path.text = cppInterface.urlToLocalStr(selectedURL)
            listView_img.currentIndex = -1
            selectedIndex = -1
        }
    }

    button_ok_img.onClicked: {
        var selectedURL = listModel_img.get(selectedIndex, "fileURL")
        if (!listModel_img.get(selectedIndex, "fileIsDir"))
            cppInterface.sendImg(selectedURL)
        imageReqFinish()
    }

    button_cancel_img.onClicked: {
        imageReqFinish()
    }

    Component.onCompleted: {
        listModel_img.folder = cppInterface.getPicturePath()
        text_path.text = cppInterface.urlToLocalStr(listModel_img.folder)
    }
}
