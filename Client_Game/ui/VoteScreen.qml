import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    Rectangle{
        id : votingScreen
        anchors.fill: parent
        color : "#0066FF"

        // voteImage = modelData : đường dẫn ảnh
        property string  voteImage: ""
        Text{
            id : titleText
            anchors{
                top : votingScreen.top
                topMargin: 40
                horizontalCenter: votingScreen.horizontalCenter
            }
            text : "Vote for the best Image!"
            font.pixelSize: 75
            font.bold: true
            color :  "black"
        }

        GridView{
            id : gridImage
            anchors{
                top : titleText.bottom
                left: votingScreen.left
                right: votingScreen.right
                bottom: button_Vote.top
                margins: 20
            }
            clip : true
            cellHeight: 410
            cellWidth: 590
            model : gameManager.drawList
            delegate: Rectangle{
                id: rectImage
                width:  585
                height : 400
                color : "#C4C4C4"
                border.color: "yellow"
                border.width: {
                    // từng phần tử trong GridView modelData là đường dẫn
                    if(votingScreen.voteImage === modelData)
                    {
                        return 5;
                    }
                    else return 0;
                }

                Image {
                    source: modelData
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                }
                MouseArea{
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        votingScreen.voteImage = modelData
                    }
                }
            }
        }

        ButtonGame{
            id : button_Vote
            anchors{
                right : gridImage.right
                rightMargin: 20
                bottom: parent.bottom
                bottomMargin: 10
            }
            width: 236
            height : 65
            buttonText: "Vote"
            buttonTextPixcelSize: 52
            onButtonClicked: {
                console.log("Had Clicked Button Vote")
                // votingScreen.voteImage : đường dẫn ảnh được chọn
                gameManager.castVote(votingScreen.voteImage)
                mainLoader.source = "qrc:/ui/WaitingForPlayrsScreen.qml"
            }
        }
    }

}
