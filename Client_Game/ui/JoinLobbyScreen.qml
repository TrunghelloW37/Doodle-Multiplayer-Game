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
                topMargin: 40
                horizontalCenter: backgroundId.horizontalCenter
            }
            text : "Enter ID Room"
            font.pixelSize: 65
            font.bold: true
            color :  "white"
        }

        TextField{
            id : inputIdRoom
            anchors{
                top : titleId.bottom
                topMargin: 35
                horizontalCenter: titleId.horizontalCenter
            }
            width: titleId.width + 20
            placeholderText: "ENTER ROOM ID"
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 50
            font.bold: true
            color: "black"
        }

        ButtonGame{
            id : button_JoinGameId
            anchors{
                top : inputIdRoom.bottom
                topMargin: 50
                horizontalCenter: inputIdRoom.horizontalCenter
            }
            width: 336
            height : 75
            buttonText: "Join Game"
            onButtonClicked: {
                console.log("Had Clicked Button Join Lobby Game")
                if(inputIdRoom.text !== "")
                {
                    console.log(inputIdRoom.text)
                    gameManager.joinLobbyRequest(inputIdRoom.text);
                }
            }
        }

        ButtonGame{
            id : button_BackId
            anchors{
                top : button_JoinGameId.bottom
                topMargin: 50
                horizontalCenter: button_JoinGameId.horizontalCenter
            }
            width: 336
            height : 75
            buttonText: "Back"
            onButtonClicked: {
                console.log("Had Clicked Button Back")
                mainLoader.source = "qrc:/ui/GameSelectScreen.qml"
            }
        }


    }
}
