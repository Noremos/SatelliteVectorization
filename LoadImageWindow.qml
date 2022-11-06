import QtQuick 2.15
import QtQuick.Controls 2.15

Popup {
    id: root
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    width: 400
    height: 350
    //    onWidthChanged: Math.max(200, width)
    //    onHeightChanged: Math.max(300, height)
    modal: true
    focus: true

    //select img
    Label {
        id: lb_img
        width: 28
        height: 18
        text: qsTr("Изображение")
        anchors.top: tf_prjPath.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: xst
    }
    TextField {
        id: tf_imgPath
        x: xst
        height: 40
        text: bt_selectImg.fileName
        anchors.right: bt_selectImg.left
        anchors.rightMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: xst
        anchors.top: lb_img.bottom
        anchors.topMargin: 5
        placeholderText: "Путь к изображению"
    }

    LoadButton {
        id: bt_selectImg
        width: 40
        height: 40
        text: qsTr("...")
        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.top: lb_img.bottom
        anchors.topMargin: 5
        //        selectFolder: false
        filter: ["Image files (*.jpg *.png *.bmp *.tif)"]
        //            onAccepted: tf_imgPath.text = bt_selectImg.fileName
    }
}
