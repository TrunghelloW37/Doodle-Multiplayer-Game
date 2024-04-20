import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id : mainMenuScreen
    Rectangle{
        id : background
        anchors.fill: parent
        color : "#0066FF"
        Text{
            id : titleGame
            anchors{
                top : background.top
                topMargin: 40
                horizontalCenter: background.horizontalCenter
            }
            text : "DoodleDrawer"
            font.pixelSize: 75
            font.bold: true
            color :  "white"
        }

        ButtonGame{
            id : buttonPlayId
            anchors.centerIn: parent
            width: 336
            height : 105
            buttonText: "Play"
            onButtonClicked: {
                console.log("Had Clicked Button Play")
                mainLoader.source = "qrc:/ui/GameSelectScreen.qml"
            }
        }

        ButtonGame{
            id : buttonQuickId
            anchors.top: buttonPlayId.bottom
            anchors.topMargin: 40
            anchors.horizontalCenter: buttonPlayId.horizontalCenter
            width: 336
            height : 105
            buttonText: "Quick"
            onButtonClicked: {
                console.log("Had Clicked Button Quick")
                Qt.quit()
            }
        }


    }

}
