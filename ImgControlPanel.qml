import QtQuick 2.15
import QtQuick.Controls 2.15
import My 1.0

BasePanel {
    id: root
    Row {
        anchors.leftMargin: 10
        id: rows
        anchors.bottom: parent.bottom
        height: 60
        spacing: 10

        Button {
            text: "Показть в отдельном окне"

            width: 150
            onClicked: backend.cntrlShowInWindow()
        }
        Button {
            text: "Зашумиь белым"

            width: 150
            onClicked: backend.cntrlNoize(230, 25, 0)
        }
        Button {
            text: "Зашумиь чёрныйм"

            width: 150
            onClicked: backend.cntrlNoize(0, 25, 255)
        }

        Button {
            text: "Сравнить размер"

            width: 150
            onClicked: lbSize.text = backend.cntrlGetCompiredSize()
        }
        Button {
            text: "Удалить линии"
            width: 100
            onClicked: backend.removeLines()
        }
    }
    Row {
        LoadButton {
            id: btn
            text: "Выгрузить"
            useOpenDialog: false
            width: 100
            onAccepted: {

                backend.cntrlExport(filePath)
            }
        }
        Label {
            id: lbSize
        }
        Button {
            text: "Убрать шум"
            onClicked: backend.removeNoise()
        }
    }
    Row {
        anchors.bottom: rows.top
        anchors.bottomMargin: 10
        Button {
            text: "Операция над регионом"

            width: 150
            onClicked: backend.cntrlLowFreq(spinbox.realValue, addSP.value)
        }
        SpinBox {
            id: addSP
            value: 0
            from: -255
            to: 255
        }

        SpinBox {
            id: spinbox
            value: 100
            from: -25500
            to: 25500
            stepSize: 50

            property int decimals: 2
            property real realValue: value / 100

            validator: DoubleValidator {
                bottom: Math.min(spinbox.from, spinbox.to)
                top: Math.max(spinbox.from, spinbox.to)
            }

            textFromValue: function (value, locale) {
                return Number(value / 100).toLocaleString(locale, 'f',
                                                          spinbox.decimals)
            }

            valueFromText: function (text, locale) {
                return Number.fromLocaleString(locale, text) * 100
            }
        }

        Button {
            text: "Сортировка"
            onClicked: backend.cntrlSortByLen()
        }

		ComboBox {
			id: compCB
			textRole: "text"
			valueRole: "value"

			model: [{
					"text": "Матричные точки",
					"value": 0
				}, {
					"text": "Начало матрицы",
					"value": 1
				}, {
					"text": "Поверх",
					"value": 2
				}]

			onCurrentIndexChanged: {

			}
		}
    }
}
