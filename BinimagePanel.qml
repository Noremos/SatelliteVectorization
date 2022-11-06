import QtQuick 2.15
import My 1.0

BasePanel {

    property alias image: image
    ImageItem {
        id: image
        objectName: "mainImage"
        //        source: "filex    :///C:/Users/Sam/Pictures/hole-test.png"
        visible: true
        anchors.fill: parent
    }
}
