import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import My 1.0

ApplicationWindow {
	id: root
	visible: true
	width: 1280
	height: 720
	minimumWidth: 300
	minimumHeight: 250
	visibility: "Maximized"

	title: qsTr("Binbar")

	FileDialog {
		id: fileDialog
		title: "Выберите файл, пожалуйста"
		selectMultiple: false
		selectExisting: true
		selectFolder: false
		onAccepted: {
			backend.loadImage(fileDialog.fileUrl.toString(
								  ).substring(8))
		}
		onRejected: {
			console.log("Canceled")
		}

		Component.onCompleted: visible = false
	}


	FileDialog {
		id: fileDialog2
		title: "Выберите файл, пожалуйста"
		selectMultiple: false
		selectExisting: true
		selectFolder: true
		onAccepted: {
			backend.setTempDir(folder.toString(
								  ).substring(8))
		}
		onRejected: {
			console.log("Canceled")
		}

		Component.onCompleted: visible = false
	}

	menuBar: MenuBar {
		   Menu {
			   title: qsTr("&Файл")
			   Action {
				    text: qsTr("&Открыть изображение...")
					onTriggered:{
						fileDialog.visible = true
					}
			   }
			   Action {
				    text: qsTr("&Выбрать каталог для времнных файлов...")
					onTriggered: {
						fileDialog2.visible = true
				}
			}
		   }
		   Menu {
			   title: qsTr("&Help")
			   Action { text: qsTr("&1. Выберите изображение\n2. Выберите методпостроения баркодов\n3. Обработайте изображение\n4. Нажмите кнопку \"Show all\" и добавляйте объекты в классификатор") }
		   }
	   }

    Topmenu {
        id: menu
        objectName: "menu"
        height: 50
        x: 10
        y: 5
        //        width: parent.width
    }

    MainZone {
        id: mainZone
        objectName: "mainZone"
        //        anchors.fill: parent
        anchors.top: menu.bottom
        height: parent.height - menu.height - menu.y
        width: parent.width

        //        anchors.bottom: root.bottom
        //        anchors.left: root.left
        //        anchors.right: root.right
        //        Button {
        //            id: button
        //            x: 112
        //            y: 56
        //            text: qsTr("Button")
        //            onClicked: backend.run2(mainZone)
        //        }
    }
    Component.onCompleted: {
		backend.setup(mainZone.image, mainZone.preprocessedImage, mainZone.slides)
    }
}

/*##^##
Designer {
	D{i:0;formeditorZoom:0.5}
}
##^##*/

