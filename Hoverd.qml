import QtQuick 2.13

MouseArea
{
    hoverEnabled: true
    anchors.fill: parent
    onPressed: mouse.accepted = false
    onEntered: cursorShape = Qt.PointingHandCursor

}
