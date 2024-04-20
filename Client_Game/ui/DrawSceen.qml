import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    Rectangle{
        id : backgroundId
        anchors.fill: parent
        color : "#0066FF"
        Text{
            id : titleId
            anchors{
                top : backgroundId.top
                topMargin: 10
                horizontalCenter: backgroundId.horizontalCenter
            }
            text : "Draw Screen"
            font.pixelSize: 45
            font.bold: true
            color :  "white"
        }

        // Rectangle để vẽ hình
        Rectangle{
            id : rect_PaintId
            anchors.centerIn: parent
            width: backgroundId.width * .90
            height: backgroundId.height * .65
            color : "gray"
            radius: 10

            // Chú ý
            Canvas{
                id : drawingCanvas
                anchors.fill: parent

                property int startX : -1
                property int startY : -1
                property int finishX: -1
                property int finishY: -1

                function clearCanvans()
                {
                    var ctx = drawingCanvas.getContext("2d");
                    ctx.clearRect(0,0,drawingCanvas.width,drawingCanvas.height);
                    requestPaint();
                }

                onPaint: {
                    var ctx = drawingCanvas.getContext("2d")
                    ctx.beginPath()
                    ctx.strokeStyle = "black"
                    ctx.lineWidth = 5
                    ctx.lineJoin = "round"
                    // Hàm này di chuyển "bút vẽ" (con trỏ vẽ) đến một vị trí cụ thể trên Canvas.
                    ctx.moveTo(startX,startY)
                    // Hàm này vẽ một đường thẳng bắt đầu từ vị trí bút vẽ hiện tại
                    ctx.lineTo(finishX,finishY)
                    startX = finishX
                    startY = finishY
                    ctx.closePath()
                    ctx.stroke()
                }

                MouseArea{
                    anchors.fill: parent
                    onPressed: {
                        drawingCanvas.startX = mouseX
                        drawingCanvas.startY = mouseY
                    }

                    onPositionChanged: {
                        //console.log("Mouse Position: " + mouseX + "-" + mouseY)
                        drawingCanvas.finishX = mouseX
                        drawingCanvas.finishY = mouseY
                        drawingCanvas.requestPaint()
                    }
                }
            }
        }

        ButtonGame{
            id : button_Clear
            anchors{
                top : rect_PaintId.bottom
                topMargin: 25
                left : rect_PaintId.left
            }
            width: 236
            height : 65
            buttonText: "Clear"
            buttonTextPixcelSize: 52
            onButtonClicked: {
                console.log("Had Clicked Button clear")
                drawingCanvas.clearCanvans()
            }
        }

        ButtonGame{
            id : button_Save
            anchors{
                top : rect_PaintId.bottom
                topMargin: 25
                right : rect_PaintId.right
            }
            width: 236
            height : 65
            buttonText: "Save"
            buttonTextPixcelSize: 52
            onButtonClicked: {
                console.log("Had Clicked Button Save")
                console.log("Path : D:\\build-Game-Desktop_Qt_5_15_2_MinGW_64_bit-Debug\\Client_Game")
                drawingCanvas.save("tmp.png")
                // WaitingForPlayrsScreen.qml sẽ gửi ảnh xuống server để xử lý
                mainLoader.source = "qrc:/ui/WaitingForPlayrsScreen.qml"
            }
        }
    }
}
