import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id : root
    Rectangle{
        id : background
        anchors.fill: parent
        color : "#0066FF"
        Text{
            id : titleGame
            width: background.width * .85
            anchors.top: background.top
            anchors.topMargin: 20
            anchors.horizontalCenter: background.horizontalCenter
            text : gameManager.winner + " was the best Image"
            font.pixelSize: 55
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
            font.bold: true
            color :  "white"
        }

        Rectangle{
            id : imageWinner
            width: parent.width * .85
            height: parent.height * .65
            anchors.centerIn: parent
            color : "gray"
            Image {
                source: gameManager.winnerImage()
                anchors.fill: parent

            }
        }

        ButtonGame{
            id : button_done
            anchors{
                right : imageWinner.right
                rightMargin: 20
                top: imageWinner.bottom
                topMargin: 30
            }

            width: 236
            height : 65
            buttonText: "Done"
            buttonTextPixcelSize: 52
            onButtonClicked: {
                console.log("Had Clicked Button Done")
                Qt.quit()
            }
        }
    }
}
