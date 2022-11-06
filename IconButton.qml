import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls 1.4 as QQC1
import QtGraphicalEffects 1.15
import QtQuick.Layouts 1.12

Button {
    id: button
    text: ""
    implicitWidth: height
    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
    property string source

    Image {
        id: image
        fillMode: Image.PreserveAspectFit
        anchors.centerIn: parent
        sourceSize.height: button.background.height - 6
        height: sourceSize.height
        source: button.source
    }
    ColorOverlay {
        anchors.fill: image
        source: image
        // In dark styles background is dark, this makes the image white
        color: tools.isDarkTheme() ? "#ffffffff" : "00000000"
    }
}
