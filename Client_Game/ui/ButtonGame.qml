import QtQuick 2.15

Rectangle {
    id : gameButtonId
    property string buttonColor: "#FFBA49"
    property string buttonText: ""
    property int buttonTextPixcelSize: 62

    signal buttonClicked()

    radius: 15
    color : buttonColor

    Text {
        id : textButtonId
        anchors.centerIn: parent
        color : "white"
        text:  buttonText
        font.pixelSize: buttonTextPixcelSize

    }

    MouseArea{
        anchors.fill: parent
        onClicked: {
            gameButtonId.buttonClicked()
        }

    }
}
