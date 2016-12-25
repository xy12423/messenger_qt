import QtQuick 2.7

PageImgForm {
    id: form_pageimg

    signal imageReqFinish()

    Connections {
        target: form_pageimg;
        onSelectedIndexChanged: {
            var selectedURL = form_pageimg.listModel_img.get(selectedIndex, "fileURL")
            if (form_pageimg.listModel_img.get(selectedIndex, "fileIsDir"))
                form_pageimg.listModel_img.folder = selectedURL
        }
    }

    Connections {
        target: button_ok_img;
        onClicked: {
            var selectedURL = form_pageimg.listModel_img.get(selectedIndex, "fileURL")
            if (!form_pageimg.listModel_img.get(selectedIndex, "fileIsDir"))
                cppInterface.sendImg(selectedURL)
            form_pageimg.imageReqFinish()
        }
    }

    Connections {
        target: button_cancel_img;
        onClicked: {
            form_pageimg.imageReqFinish()
        }
    }

    Component.onCompleted: {
        form_pageimg.listModel_img.folder = cppInterface.getPicturePath()
    }
}
