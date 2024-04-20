import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id : root
    Rectangle{
        id : background
        anchors.fill: parent
        color : "#23001E"

        Component.onCompleted: {
            gameManager.drawDone()
        }

        Text{
            id : titleGame
            width: background.width * .85
            anchors{
                centerIn: parent
            }
            text : "Waiting for other players to finished..."
            font.pixelSize: 75
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            color :  "white"
        }
    }
}
