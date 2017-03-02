import QtQuick 2.8

PageKeyManForm {
    signal reqImport()
    signal reqExport(int keyID)
    signal reqFinish()

    Connections {
        target: cppInterface
        onRefreshKeylist: {
            listModel_keyman.clear()
            for (var i = 0; i < key.length; i++)
                listModel_keyman.append({ name: key[i], info: ex[i] })
            selectedIndex = -1
        }
    }

    onSelectedIndexChanged: {
        if (selectedIndex != -1)
            text_keyex.text = listModel_keyman.get(selectedIndex).info
    }

    button_trust.onClicked: {
        cppInterface.trustKey()
    }

    button_distrust.onClicked: {
        if (selectedIndex != -1)
            cppInterface.distrustKey(selectedIndex)
    }

    button_import.onClicked: {
        reqImport()
    }

    button_export.onClicked: {
        if (selectedIndex != -1)
            reqExport(selectedIndex)
    }

    button_save.onClicked: {
        if (selectedIndex != -1)
        {
            cppInterface.modifyKey(selectedIndex, text_keyex.text)
            listModel_keyman.setProperty(selectedIndex, "info", text_keyex.text)
        }
    }

    button_back.onClicked: {
        reqFinish()
    }

    Component.onCompleted: {
        cppInterface.reqKeylist()
    }
}
