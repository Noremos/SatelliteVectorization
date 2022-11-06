import QtQuick 2.15
import My 1.0

BasePanel {

    property alias image: image
    ImageItem {
        id: image
        objectName: "image"
        //        source: "file:///D:/hole-test.png"
        visible: true
        anchors.fill: parent
    }
}
