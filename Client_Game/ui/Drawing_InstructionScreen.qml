import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    Rectangle{
        id : backgroundId
        anchors.fill: parent
        color : "#0066FF"
        // Text chủ đề
        Text{
            id : titleInstrctuonId
            anchors{
                top : backgroundId.top
                topMargin: 10
                horizontalCenter: backgroundId.horizontalCenter
            }
            text : "Draw: "  + gameManager.drawInstruction + "!"
            font.pixelSize: 45
            font.bold: true
            color :  "white"
        }

        Rectangle{
            id : rect_PaintId
            anchors.centerIn: parent
            width: backgroundId.width * .90
            height: backgroundId.height * .65
            color : "gray"
            radius: 10

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

                Component.onCompleted: {
                    console.log("Had Loader in Componene.onCompleted: Drawing_InstructionScreen!!")
                    drawingCanvas.loadImage(gameManager.imageFilePath())
                }

                onImageLoaded: {
                    console.log("QML : Had Image Loader  Drawing_InstructionScreen")
                    var ctx = drawingCanvas.getContext("2d");
                    ctx.drawImage(gameManager.imageFilePath(),0,0)
                    drawingCanvas.requestPaint()
                }

                onPaint: {
                    var ctx = drawingCanvas.getContext("2d")
                    ctx.beginPath()
                    ctx.strokeStyle = "red"
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
            buttonText: "Done"
            buttonTextPixcelSize: 52
            onButtonClicked: {
                console.log("Had Clicked Button Done")
                drawingCanvas.save("tmp.png")
                mainLoader.source = "qrc:/ui/WaitingForPlayrsScreen.qml"
            }
        }
    }
}

