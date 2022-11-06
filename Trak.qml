import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    property alias max: slider.to
    property int index: slider.value
    id: root
    Button {
        id: prev
        enabled: slider.value != 0
        text: "Предыдущий"
        //        display: AbstractButton.IconOnly
        anchors.left: parent.left
        width: 50
        height: root.height
        //        icon.source: "back.png"
        onClicked: {
            slider.value -= 1
            setActive()
        }
    }
    Slider {
        id: slider
        anchors.left: prev.right
        anchors.right: next.left
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        height: root.height
        onValueChanged: setActive()
        to: 0
    }

    Button {
        id: next
        enabled: slider.value != slider.to
        text: "Следующий"

        //        display: AbstractButton.IconOnly
        //        icon.source: "next.png"
        anchors.right: parent.right
        width: 50
        height: root.height
        onClicked: {
            slider.value += 1
            setActive()
        }
    }
    height: 50

    function setActive() {
        backend.loadBaritem(slider.value)
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:1.5}
}
##^##*/

