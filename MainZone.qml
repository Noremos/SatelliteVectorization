import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import My 1.0

Item {
    id: mainZone
	property alias image: mainImage
	property alias preprocessedImage: processedImage
	property alias slides: slides
	width: 500
	height: 500

	Item {
		id: topIamges

		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: slides.top
		anchors.rightMargin: 0

		ImagePanel {
			id: mainImage
			width: parent.width / 2
			objectName: "mainImage"

			anchors.left: parent.left
			anchors.top: parent.top
			anchors.bottom: parent.bottom
		}

		Item
		{
			id: prepImg
			anchors.left: mainImage.right
			anchors.right: parent.right
			anchors.top: parent.top
			anchors.bottom: parent.bottom
			ImageItem {
				id: processedImage
				anchors.fill: prepImg

	//			anchors.left: mainImage.right
	//			anchors.right: parent.right
	//			anchors.top: parent.top
	//			anchors.bottom: parent.top
				MouseArea {
					id: selectionMouseArea
					property int initialXPos
					property int initialYPos
					property bool justStarted

					anchors.fill: parent
					z: 2 // make sure we're above other elements
					onPressed: {
						if (mouse.button == Qt.LeftButton) {
							console.log("Mouse area shift-clicked !")
							            backend.click(mouse.x, mouse.y, slides.extra)
						}
					}
				}
			}
		}
	}

    SliderPanel {
		id: slides
		image: processedImage

		height: 60
		border.color: "#1e009d"
		objectName: "sliderPanel"
		anchors.top: mainImage.topIamges
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.right: parent.right
    }

}


