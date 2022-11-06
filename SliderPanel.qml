import QtQuick 2.15
import QtQuick.Controls 2.15
import My 1.0

BasePanel
{
    id: root
	property ImageItem image
	property alias extra: valeExtra.text

	Row{
		leftPadding: 20
		topPadding: 10

		spacing: 20
        Button {
			id: next
			text: "Обновить"
			icon.source: "qrc:/imgs/update.png"
			display: AbstractButton.TextBesideIcon
			width: 100
            onClicked: {
				let back = valeExtra.text;
				valeExtra.text += "json;";
				updateSI(true);
				valeExtra.text = back;
			}
        }

		Button {
			id: addClass
			text: "Add class"
			width: 100
			onClicked: {
				backend.addClass(valeClass.text * 1)
			}
		}

		TextField  {
			id: valeClass
			text: "0"
			width: 30
			validator: IntValidator {bottom: 0; top: 5}
		}

		Button {
			text: "Undoadd"
			width: 100
			onClicked: {
				backend.undoAddClass()
			}
		}

		CheckBox{
			text: "Show all"
			onCheckStateChanged: {
				if (checked)
				{
					valeExtra.text += "show0;";
				}
				else
				{
					valeExtra.text = valeExtra.text.replace("show0;", "");
				}

				updateSI(true);
			}
		}

		LoadButton {
			id: expords
			text: "Выгрузить"
			selectFolder: true
			width: 100
			height: 30

			onAccepted: {
				backend.exportResult(filePath)
			}
		}

		TextField  {
			id: valeExtra
			text: "barclass;"
			width: 300
			visible: false
		}
    }

    function updateSI(force) {
        if (force || realTime.checked)
			backend.processMain(image, valeExtra.text)
    }
}

/*##^##
Designer {
	D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
