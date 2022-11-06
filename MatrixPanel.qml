import QtQuick 2.15
import QtQuick.Controls 2.15
import My 1.0

BasePanel {
    property alias matrix: matrix
    //    Mtrx {
    //        id: matrix
    //        objectName: "mtrx"
    //        anchors.top: parent.top
    //        anchors.left: parent.left
    //        anchors.right: parent.right
    //        anchors.bottom: track.top
    //    }
    TableView {
        id: matrix
        objectName: "mtrx"
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: track.top
        columnSpacing: 1
        rowSpacing: 1
        clip: true
        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOn
            width: 20
        }
        ScrollBar.horizontal: ScrollBar {
            policy: ScrollBar.AlwaysOn
            height: 20
        }

        //        model: ListModel {
        //            id: fruitModel

        //            ListElement {
        //                name: "Apple"
        //                cost: 2.45
        //            }
        //            ListElement {
        //                name: "Orange"
        //                cost: 3.25
        //            }
        //            ListElement {
        //                name: "Banana"
        //                cost: 1.95
        //            }
        //        }
        model: Commatrix {
            id: commatrix
            objectName: "model"
        }
        //    rows: 10
        //    columns: 10
        delegate: Rectangle {
            implicitWidth: 30
            implicitHeight: 30

            //            border.color: red
            //            border.width: 1
            TextField {
                text: display.toString()
                //                inputMethodHints: Qt.ImhDigitsOnly
                anchors.fill: parent
                leftPadding: 5
                rightPadding: 5
                //                horizontalAlignment: TextInput.AlignHCenter
                //                inputMethodComposing :true
                //                width: 10
                //                color:
                //                onTextChanged: text = text.replace(/\D+/g, '')
            }
        }
    }
    Trak {
        id: track
        objectName: "track"

        height: 50
        anchors.left: parent.left
        anchors.right: setZero.left
        anchors.bottom: parent.bottom
    }
    Button {
        id: setZero
        text: "Обнулить"
        width: 30
        onClicked: backend.zeroing()
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    function updateMtrx() {
        matrix.update()
    }
}
