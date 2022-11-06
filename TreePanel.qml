import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4 as QQC1
import QtQml.Models 2.2 as QQC1MODELS
import My 1.0

BasePanel {

    TreeModel {
        id: model
        objectName: "model"

        //              model: myModel
    }
    QQC1MODELS.ItemSelectionModel {
        id: selModel
        model: model
        onSelectionChanged: {
            var ind = selModel.currentIndex
            backend.spotBarItem(ind)
        }
    }
    QQC1.TreeView {
        id: barTree
        objectName: "tree"

        height: parent.height
        width: parent.width

        model: model
        selection: selModel
        QQC1.TableViewColumn {
            role: "name"
            title: "Barcode"
        }

        itemDelegate: Item {
            Text {
                width: 10
                text: styleData.value
                //                text: styleData.value.indentation + ": " + styleData.value.text
                //                text: display.toString()
                anchors.fill: parent
                color: styleData.textColor
                elide: styleData.elideMode
            }
        }
        QQC1.TableViewColumn {
            role: "visible"
            title: "Visability"
        }
        //        onSelectionChanged: {
        //            var ind = selModel.currentIndex
        //            backend.spotBarItem(ind)
        //        }
        //        onClicked: {
        //            var ind = selModel.currentIndex
        //            backend.spotBarItem(ind)
        //        }

        //        selection: ItemSelectionModel {
        //        }
        //        QQC1.TableViewColumn {
        //            role: "name"
        //            title: "Object"
        //        }
    }
    //RoundButton { // Qt >= 5.8.0
    //    IconButton {}
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

