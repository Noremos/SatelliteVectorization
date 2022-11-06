import QtQuick 2.13
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.3

Rectangle {
    id: root
    property string text: "Загрузить"
    property alias filter: fileDialog.nameFilters
    property string fileName: (selectFolder ? fileDialog.folder.toString(
                                                  ).substring(
                                                  8) : fileDialog.fileUrl.toString(
                                                  ).substring(8))
    property alias fileNames: fileDialog.fileUrls
    property alias multipleSelect: fileDialog.selectMultiple
    property alias selectFolder: fileDialog.selectFolder

    //signal accepted
    signal accepted(var filePath)
    width: 200
    height: 50
    Button {
        id: loadBtn
        anchors.fill: root
        text: root.text
        onClicked: fileDialog.visible = true
        anchors.centerIn: root
        Hoverd {}
    }
    FileDialog {
        id: fileDialog
        title: "Выберите файл, пожалуйста"
        //folder: shortcuts.home
        selectMultiple: false
        selectExisting: true
        onAccepted: root.accepted(fileDialog.fileUrl)
        onRejected: {
            console.log("Canceled")
        }

        Component.onCompleted: visible = false
    }
}
