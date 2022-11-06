import QtQuick 2.15
import QtQuick.Controls 2.15

import QtCharts 2.15

BasePanel {
    property alias barchart: barchart
    property int maxhei: 0
    ChartView {
        id: barchart
        objectName: "barchart"
        title: "barcode"
        anchors.fill: parent
        //        anchors.bottom: buts.top
        antialiasing: true
        animationOptions: ChartView.NoAnimation
        dropShadowEnabled: false
        ValueAxis {
            id: number
            max: maxhei + 1
        }

        ValueAxis {
            id: period
            min: 0
            max: 255
            tickCount: 50
            //            tickInterval: 5
            //            tickType: ValueAxis.TicksFixed
        }
        //        LineSeries {
        //            name: "line1"
        //        }
        Row {
            id: buts
            Button {
                text: "Отсортировать"
                onClicked: backend.reloadBarcode(true)
                anchors.bottom: parent.bottom
            }
        }
    }

    function addBarcode(bar) {
        for (var i = 0; i < bar.length; i += 2)
            addLine(i + 1, bar[i], bar[i + 1])
    }

    function addLine(ind, x, len) {
        var ser = barchart.createSeries(ChartView.SeriesTypeLine,
                                        "Line" + ind.toString(), period, number)
        ser.append(x, ind)
        ser.append(x + len, ind)
        ser.color = Qt.rgba(0, 0, 1, 1)
        ser.useOpenGL = true
        maxhei = Math.max(maxhei, ind)
        //        barchar.add
    }

    function setActive(oldInd, newInd) {
        barchart.series(oldInd).width = 2.0
        barchart.series(oldInd).color = Qt.rgba(0, 0, 1, 1)
        barchart.series(newInd).width = 4.0
        barchart.series(newInd).color = Qt.rgba(1, 0, 0, 1)
    }

    //    function addBarcode(lines) {
    //        for (line in lines) {
    //            addLine(line.x, line.len)
    //        }
    //    }
    function clearAll() {
        barchart.removeAllSeries()
    }
}
