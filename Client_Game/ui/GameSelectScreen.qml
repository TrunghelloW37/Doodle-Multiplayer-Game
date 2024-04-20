import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id : gameSelectId
    Rectangle{
        id : backgroundId
        anchors.fill: parent
        color : "#0066FF"
        Text{
            id : titleGame
            anchors{
                top : backgroundId.top
                topMargin: 40
                horizontalCenter: backgroundId.horizontalCenter
            }
            text : "DoodleDrawer"
            font.pixelSize: 75
            font.bold: true
            color :  "white"
        }

        Column{
            id : columnId
            anchors.centerIn: parent

            spacing : 40
            ButtonGame{
                id : button_CreateGameId
                width: 336
                height : 75
                buttonText: "Create"
                onButtonClicked: {
                    console.log("Had Clicked Button Create")
                    gameManager.createGameRequest()
                }
            }
            ButtonGame{
                id : button_JoinGameId
                width: 336
                height : 75
                buttonText: "Join"
                onButtonClicked: {
                    console.log("Had Clicked Button Join")
                    mainLoader.source = "qrc:/ui/JoinLobbyScreen.qml"
                }
            }

            ButtonGame{
                id : button_BackId
                width: 336
                height : 75
                buttonText: "Back"
                onButtonClicked: {
                    console.log("Had Clicked Button Back")
                    mainLoader.source = "qrc:/ui/MainMenuScreen.qml"
                }
            }
        }


    }
}
