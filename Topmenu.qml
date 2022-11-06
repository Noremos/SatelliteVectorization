import QtQuick 2.15
import QtQuick.Controls 2.15
import My 1.0
import MyEnums 1.0
import QtQuick.Dialogs 1.3

Row {
	id: row
	leftPadding: 20
	spacing: 10
    LoadButton {
        id: lb
		text: "Загурзить изобр."
		onAccepted: backend.loadImage(filePath)
		width: 150
		height: procCB.height
    }

	ComboBox {
		id: compCB
		textRole: "text"
		valueRole: "value"

		model: [{
				"text": "Компонента",
				"value": 0
			}
//			, {
//				"text": "Дыра",
//				"value": 1
//			}
		]
	}

    ComboBox {
        id: procCB
//		enabled: compCB.currentIndex === 0
        textRole: "text"
		width: 120
        valueRole: "value"
		currentIndex: 2
        model: [{
                "text": "От 0 до 255",
				"value": 0
            }, {
                "text": "С 255 до 0",
				"value":1
			}, {
				"text": "По расстоянию",
				"value":2
			},
			{
				"text": "Инвертировать",
				"value": 3
			},
			{
				"text": "Радар",
				"value": 4
			},
			{
				"text": "Тру растоя",
				"value": 6
			}
		]
    }

    ComboBox {
        id: colorCB
		width: 150
        textRole: "text"
        valueRole: "value"
		model: [
			{
				"text": "Как в изображении",
				"value": 0
			},
			{
                "text": "Сервый",
				"value": 1
			}, {
                "text": "RGB",
				"value": 2
			}]
    }

    Button {
		text: "Построить баркод"
		onClicked: backend.run(procCB.currentValue, colorCB.currentValue,
							   compCB.currentValue)
	}


    FileDialog {
        id: saveDialog
        objectName: "saveDialog"
        nameFilters: ["Image files (*.jpg *.png *.bmp *.tif)"]
        title: "Выберите путь для сохранения"
        //                    folder: backend.prjPath ///" //Here you can set your default folder
        selectExisting: false
        defaultSuffix: "png"
        selectMultiple: false

        Component.onCompleted: visible = false
    }
	Button
	{
		text:"Восстановить"
		onCanceled: backend.restoreSource()
	}

}

/*##^##
Designer {
	D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
