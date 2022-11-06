import QtQuick 2.15
import My 1.0

BasePanel {
    id: root
    property alias image: mainImage
    ImageItem {
        id: mainImage
        objectName: "mainImage"
        //        source: "filex    :///C:/Users/Sam/Pictures/hole-test.png"
        visible: true
        anchors.fill: parent
        Rectangle {
            id: selectionRect
            visible: false
            x: 0
            y: 0
            z: 99
            width: 0
            height: 0
            rotation: 0
            color: "#5F227CEB"
            border.width: 1
            border.color: "#103A6E"
            transformOrigin: Item.TopLeft
        }

        ImageItem {
            id: mask
            objectName: "maskImage"
            anchors.fill: parent
        }
    }

//	MouseArea {
//		id: selectionMouseArea
//		property int initialXPos
//		property int initialYPos
//		property bool justStarted

//		anchors.fill: parent
//		z: 2 // make sure we're above other elements
//		onPressed: {
//			if (mouse.button == Qt.LeftButton) {
//				console.log("Mouse area shift-clicked !")
//				            backend.click(selectionRect.x, selectionRect.y)
//			}
//		}
//	}
}
