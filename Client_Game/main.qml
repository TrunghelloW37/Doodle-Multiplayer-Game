import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15

Window {
    width: 1280
    height: 720
    visible: true
    title: qsTr("Hello World")

    Connections{
        target: gameManager
        function onInGameLobby(){
            mainLoader.source = "qrc:/ui/LobbyScreen.qml"
        }

        function onGameStarting(){
            mainLoader.source = "qrc:/ui/DrawSceen.qml"
        }

        function onDrawInstructionTime(){
            mainLoader.source = "qrc:/ui/Drawing_InstructionScreen.qml"
        }

        function onVotingTime(){
            mainLoader.source = "qrc:/ui/VoteScreen.qml"
        }

        function onGameOver(){
            console.log("QML : Had received emit GameOver")
            mainLoader.source = "qrc:/ui/WinnerScreen.qml"
        }
    }

    Loader{
        id : mainLoader
        anchors.fill: parent
        source : "qrc:/ui/MainMenuScreen.qml"
    }


}
