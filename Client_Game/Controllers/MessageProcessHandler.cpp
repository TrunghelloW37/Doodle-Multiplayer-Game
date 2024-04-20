#include "MessageProcessHandler.h"
#include <QDebug>
MessageProcessHandler::MessageProcessHandler(QObject *parent)
    : QObject{parent}
{

}

// Xử lý tin nhắn nhận được
void MessageProcessHandler::processMessage(QString message)
{

    // Đăng kí Id cho Client (5555 là ví dụ Id thôi - là số tù 1000 - 9999)
    // type:uniqueId;payLoad:5555;clientList:1234,4444,5555
    QStringList separated = message.split(QRegExp(";"));
    // 1 Đăng kí Id cho Client
    if(separated.first() == "type:uniqueId")
    {
        qDebug() << "1. Client App: unique Id registration: ";
        separated.pop_front();
        if(separated.first().contains("payLoad:"))
        {
            QString newClientId = separated.first();
            newClientId = newClientId.remove("payLoad:"); // Xóa "payLoad:" để Lấy Id thôi
            // uniqueId Registration : Đăng kí Id duy nhất
            emit uniqueIdRegistration(newClientId);
        }
    }
    // 2. type:newLobbyCreated;payLoad:5555;clientList:1234
    //    type:joinSuccess;payLoad:5555;clientList:1234.4444,5555...
    else if(separated.first() == "type:newLobbyCreated" || separated.first() == "type:joinSuccess")
    {
        qDebug() << "2. Client App: Client Join Room Game: " << message;
        separated.pop_front(); // bỏ phần tử đầu tiên
        QString newLobbyId = QString(); // newLobbyId là Id phòng
        QStringList lobbyClient = QStringList(); // lobbyClient là Qstring chứa các clients trong room
        if(separated.first().contains("payLoad:"))
        {
            newLobbyId = separated.first();
            newLobbyId = newLobbyId.remove("payLoad:");
        }
        separated.pop_front();
        if(separated.first().contains("clientList:"))
        {
            QString clients = separated.first();
            clients = clients.remove("clientList:");
            lobbyClient = clients.split(QRegExp(",")); // ["1234","5678","4444","5555"]
        }

        if(newLobbyId != QString() && lobbyClient != QStringList())
        {
            qDebug() <<"2. Client App: Client in Room: " << lobbyClient;
            emit newLobby(newLobbyId,lobbyClient);
        }
    }
    // 3. "type:updatedClientList;payLoad:1234,4444,5555"
    // Hàm này để cập nhật thông tin client Id của các người chơi tham gia vào
    else if(separated.first() == "type:updatedClientList")
    {
        qDebug() << "3. Client App: Received Update client list: " << message;
        separated.pop_front();
        QString payLoad = separated.first();
        payLoad = payLoad.remove("payLoad:");
        emit lobbyListUpdated(payLoad.split(QRegExp(",")));
    }
    //4. "type:lobbyMessage;payLoad:Trung Nguyen;sender:1884"
    // Hàm xử lý đoạn chat giữa các client
    else if(separated.first() == "type:lobbyMessage")
    {
        QString newMessage; //  nội dụng tin nhắn
        QString senderId; // Ngưởi gửi tin nhắn
        separated.pop_front();
        if(separated.front().contains("payLoad:"))
        {
            newMessage = separated.front();
            newMessage = newMessage.remove("payLoad:");
        }
        separated.pop_front();
        if(separated.front().contains("sender:"))
        {
            senderId = separated.front();
            senderId = senderId.remove("sender:");
        }
        QString displayMessage = senderId + ": " + newMessage;
        emit newLobbyMessage(displayMessage);
    }
    // 5. "type:readyListChanged;payLoad:1111,2222,6666" (1111,2222,6666 are player had ready)
    // các client đã sẵn sàng ( đã nhấn nút ready)
    else if(separated.first() == "type:readyListChanged")
    {
        qDebug() <<"5. Client App : readyListChanged" << message;
        separated.pop_front();
        if(separated.front().contains("payLoad:"))
        {
            QString payLoad = separated.front();
            payLoad = payLoad.remove("payLoad:");
            QStringList readyClient = payLoad.split(QRegExp(","));
            qDebug() << "5. Client APP : List Client Ready: " << readyClient;
            emit readyListChanged(readyClient);
        }
    }
    //6. type:gameReadyToBegin;
    // Phát ra tín hiệu khi tất cả client đã sẵn sàng
    else if(separated.first() == "type:gameReadyToBegin")
    {
        qDebug() <<"6. Client App: All Client had ready";
        emit gameStarted();
    }
    // 7.type:drawingInstruction;payLoad:imageData;instruction:dog
    // Phát ra tín hiệu để gửi đi giá trị dataImage và instructuion (chủ đề hình vẽ)
    else if(separated.first() == "type:drawingInstruction")
    {
        qDebug() <<"7. Client App: received drawing instruction!";
        QString payLoad = QString(); // dữ liệu ảnh
        QString drawingInstruction = QString(); // chủ đề
        separated.pop_front();
        if(separated.first().contains("payLoad:"))
        {
            payLoad = separated.first();
            payLoad = payLoad.remove("payLoad:");
        }
        separated.pop_front();
        if(separated.first().contains("instruction:"))
        {
            drawingInstruction = separated.first();
            drawingInstruction = drawingInstruction.remove("instruction:");
        }

        if(payLoad != QString() && drawingInstruction != QString())
        {
            emit dataImageAndInstructuon(payLoad,drawingInstruction);
        }
    }
    // 8. type:gameImages;payLoad:dataImage1,dataImage2,dataImage3;clients:1111,2222,3333
    else if(separated.first() == "type:gameImages") //
    {
        qDebug()<<"8. Client app : GameImages";
        separated.pop_front();
        QString payLoad = QString(); // chứa tất cả dữ liệu ảnh - payLoad:dataImage1,dataImage2,dataImage3
        QString clients = QString(); // chứa tất cả các clientId - clients:1111,2222,3333

        if(separated.first().contains("payLoad:"))
        {
            payLoad = separated.first();
            payLoad = payLoad.remove("payLoad:");
        }
        separated.pop_front();
        if(separated.first().contains("clients:"))
        {
            clients = separated.first();
            clients = clients.remove("clients:");
        }

        if(payLoad != QString() && clients != QString())
        {
            emit gameDrawAllReady(payLoad.split(QRegExp(",")) , clients.split(QRegExp(",")));
            qDebug() << "Client App: Had emit gameDrawReady";
        }
    }
    //9. type:winnerChosen;payLoad:winner
    else if(separated.first() == "type:winnerChosen")
    {
        QString winner = QString();
        separated.pop_front();
        if(separated.first().contains("payLoad:"))
        {
            winner = separated.first();
            winner = winner.remove("payLoad:");
        }

        if(winner != QString())
        {
            emit winnerChosen(winner);
        }
    }
}
