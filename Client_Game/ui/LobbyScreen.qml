import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    Rectangle {
        id: backgroundId
        anchors.fill: parent
        color: "#23001E"


        Text {
            id: titleLobbyGame
            anchors {
                top: backgroundId.top
                topMargin: 40
                horizontalCenter: backgroundId.horizontalCenter
            }
            text: "Lobby Code:" + gameManager.roomLobbyCode
            font.pixelSize: 55
            font.bold: true
            color: "white"
        }

        // Rectangle chứa Id các người chơi tham gia vào
        Rectangle{
            id : romLobbyListBackground
            width: 340
            height: 415
            radius: 10
            color : "gray"
            anchors{
                top: titleLobbyGame.bottom
                topMargin: 5
                left: backgroundId.left
                leftMargin: 10
            }

            ListView{
                id : roomLobbyListView
                anchors.fill: romLobbyListBackground
                model : gameManager.clientsInLobby
                spacing : 5
                clip : true
                header: Rectangle{
                    anchors.horizontalCenter:  parent.horizontalCenter
                    anchors.margins: 15
                    radius: 20
                    width: roomLobbyListView.width - 15
                    height : myId.implicitHeight + 5
                    color : "yellowgreen"
                    Text {
                        id : myId
                        anchors.centerIn: parent
                        text : "My ID: " + gameManager.myId()
                        color : "white"
                        font.pixelSize: 36
                        font.bold: true
                    }

                }

                delegate: Rectangle{
                    id : rectangleId
                    anchors.horizontalCenter:  parent.horizontalCenter
                    anchors.margins: 5
                    width: roomLobbyListView.width - 15
                    height : textClientId.implicitHeight + 5
                    color: "gray"
                    radius: 10
                    Text {
                        id : textClientId
                        anchors.centerIn: parent
                        color : "white"
                        text: modelData
                        font.pixelSize: 36
                        font.bold: true
                    }

                    Image {
                        id: checkImage
                        visible: gameManager.isClientReady(modelData) // modelData : chính là clientId
                        Connections{
                            target :gameManager
                            function onReadyClientListChanged(){
                                checkImage.visible = gameManager.isClientReady(modelData)
                            }
                        }

                        width: 40
                        height: 40
                        anchors{
                            left: textClientId.right
                            leftMargin: 10
                            verticalCenter: textClientId.verticalCenter
                        }
                        fillMode: Image.PreserveAspectCrop
                        source: "qrc:/Images/check.png"
                    }
                }
            }
        }

        // Button Ready
        ButtonGame{
            id : button_ReadyId
            property bool check: false

            width: 336
            height : 85
            anchors{
                horizontalCenter: romLobbyListBackground.horizontalCenter
                top : romLobbyListBackground.bottom
                topMargin: 10
            }
            buttonText: "Ready"
            onButtonClicked: {
                check = !check
                console.log("check: " + check)
                console.log("Had Clicked Button Ready")
                gameManager.readyToPlay(check)
                button_ReadyId.buttonText =  check ? "Cannel"   : "Ready"
                button_ReadyId.buttonColor = check ? "#FF3399"	: "#FFBA49"
            }
        }

        // Rectangle chat
        Rectangle{
            id : lobbyChatBackground
            width: backgroundId.width - romLobbyListBackground.width - 50
            height: 415
            radius: 10
            color : "gray"
            anchors{
                left: romLobbyListBackground.right
                leftMargin: 20
                verticalCenter: romLobbyListBackground.verticalCenter
            }
        }

        // ListView Chat
        ListModel{
            id : myListModelChatId
        }

        Component{
            id : delegateOtherClientId
            Rectangle{
                width: listViewChat_Id.width
                height : 60
                color : "lightgreen"
                border.color : "black"
                border.width: 1
                radius: 5
                Text {
                    id : messageId
                    anchors{
                        top : parent.top
                        topMargin: 10
                        left : parent.left
                        leftMargin: 20
                    }
                    text: model.text
                    font.pixelSize: 25
                    font.family: "Arial"
                    font.bold: true
                }

                Text {
                    id: timeId
                    anchors{
                        bottom : parent.bottom
                        bottomMargin: 10
                        right : parent.right
                        rightMargin: 15

                    }
                    text: model.time
                    font.bold: true
                }

            }
        }

        // get Time
        function dataTime(){
            let date = new Date();
            return date.getHours().toString() + ":" + date.getMinutes().toString()
        }
        //
        function checkSender(message){
            let s = message.substring(0,4);
            if(s === gameManager.myId())
            {
                return true;
            }
            else return false;
        }

        ListView{
            id: listViewChat_Id
            anchors.fill: lobbyChatBackground
            model:  myListModelChatId
            spacing : 5
            delegate: delegateOtherClientId
        }



        Connections{
            target : gameManager
            function onNewLobbyMessage(message){
                myListModelChatId.append({text:message,time:backgroundId.dataTime()})
            }
        }

        // Nhập tin nhắn
        TextField{
            id : textFieldId
            anchors{
                top : lobbyChatBackground.bottom
                topMargin: 10
                left: lobbyChatBackground.left
            }
            width: lobbyChatBackground.width * .80
            height : 85
            font.pixelSize: 20
            placeholderText: "Text..."
            onAccepted: {
                gameManager.sendMessageToLobby(textFieldId.text)
                textFieldId.text = ""
            }
        }

        // Button Send Message
        ButtonGame{
            id : button_SendId
            width: lobbyChatBackground.width - textFieldId.width - 10
            height : 85
            anchors{
                verticalCenter: textFieldId.verticalCenter
                top : lobbyChatBackground.bottom
                topMargin: 10
                right : lobbyChatBackground.right
            }

            buttonText: "Send"
            onButtonClicked: {
                console.log("Had Clicked Button Send")
                gameManager.sendMessageToLobby(textFieldId.text)
                textFieldId.text = ""
            }
        }


    }
}
