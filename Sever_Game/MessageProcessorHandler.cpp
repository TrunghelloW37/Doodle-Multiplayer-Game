#include "MessageProcessorHandler.h"
#include <QDebug>
MessageProcessorHandler::MessageProcessorHandler(QObject *parent)
    : QObject{parent}
{

}

// Xử lý tin nhắn
void MessageProcessorHandler::processMessage(QString message)
{
    // type:CreateGame; payLoad:0; sender : 5000
    QStringList separated = message.split(QRegExp(";"));
    if(separated.first() == "type:createGame")
    {
        // create game
        qDebug() << "Mess1.Create game request"; // request : yêu cầu
        separated.pop_front(); // Xóa phần tử nhất
        separated.pop_front();
        if(separated.first().contains("sender:"))
        {
            // SendId là Id Client muốn tạo Create game
            QString senderId = separated.first();
            senderId = senderId.remove("sender:");
            emit createGameRequest(senderId);
        }
    }
    // type:joinGame; payLoad:4000;sender:5000
    else if(separated.first() == "type:joinGame")
    {
        // joinGame
        qDebug() << "Mess2. Join game request";
        separated.pop_front();
        QString lobbyId = QString(); //  Id phòng muốn tham gia vào
        QString senderId = QString(); //  sender : Id client (người gửi yêu cầu vào phòng)
        if(separated.first().contains("payLoad:"))
        {
            lobbyId = separated.first();
            lobbyId = lobbyId.remove("payLoad:");
        }
        separated.pop_front();
        if(separated.front().contains("sender:"))
        {
            senderId = separated.first();
            senderId = senderId.remove("sender:");
        }

        if(lobbyId != QString() && senderId != QString())
        {
            emit joinGameLobbyRequest(lobbyId,senderId);
        }
    }
    // 3. SendMessage to Lobby
    // type:messageGame;payLoad:"helloWordl";lobbyId:5999;sender:5000
    else if(separated.first() == "type:messageGame")
    {
        // message to lobby
        qDebug() << "Mess3. Message Chat Box Request" << message;
        QString payLoad = QString(); // nội dung tin nhắn
        QString lobbyId = QString(); // Id phòng
        QString senderId = QString(); // người gửi
        separated.pop_front();
        if(separated.front().contains("payLoad:"))
        {
            payLoad = separated.front();
            payLoad = payLoad.remove("payLoad:");
        }
        separated.pop_front();
        if(separated.front().contains("lobbyID:"))
        {
            lobbyId = separated.front();
            lobbyId = lobbyId.remove("lobbyID:");
        }

        separated.pop_front();
        if(separated.front().contains("sender:"))
        {
            senderId = separated.front();
            senderId = senderId.remove("sender:");
        }

        if(payLoad != QString() && lobbyId != QString() && senderId != QString())
        {
            // Nội dung tin nhắn -- Id phòng -- Id người gửi
            emit messageLobbyToRequest(payLoad,lobbyId,senderId);
        }
    }
    // 4. type:readyToPlay;payLoad:true;sender:1111
    else if(separated.first() == "type:readyToPlay")
    {
        qDebug() << "Mess4 : readToPlay";
        QString payLoad = QString();
        QString clientId = QString();
        separated.pop_front();
        if(separated.first().contains("payLoad:"))
        {
            payLoad = separated.first();
            payLoad = payLoad.remove("payLoad:");
        }
        separated.pop_front();
        if(separated.first().contains("sender:"))
        {
            clientId = separated.first();
            clientId = clientId.remove("sender:");
        }
        if(payLoad != QString() && clientId != QString())
        emit clientReadyToPlay(payLoad,clientId);
    }
    // 5.type:imageData;payLoad:dataImage;sender:1111 ( ảnh mới bắt đầu chưa  hoàn thiện của các client)
    // 6.type:drawData;payLoad:dataImage;sender:clientId ( ảnh sau khi đã hoàn thiện của các client)
    // "type:imageData;payLoad:" + dataImage.toHex() +";sender:" + m_clientId;
    // Ảnh chưa được hoàn thiện ( mới vẽ --> imageData)
    // Ảnh đã được vẽ hoàn thiện theo đề tài ( đã vẽ xong ---> drawData)
    else if(separated.first() == "type:imageData" || separated.first() == "type:drawData")
    {
        QString dataImage = QString(); // dữ liệu hình ảnh
        QString clientID = QString(); // clientId của hình ảnh
        QString type = separated.front();
        type = type.remove("type:");

        separated.pop_front();
        if(separated.front().contains("payLoad:"))
        {
            dataImage = separated.front();
            dataImage = dataImage.remove("payLoad:");
        }
        separated.pop_front();
        if(separated.front().contains("sender:"))
        {
            clientID = separated.front();
            clientID = clientID.remove("sender:");
        }

        if(dataImage != QString() && clientID != QString() && type == "imageData")
        {
            // Gửi tín hiệu dataImage(dữ liệu hình) và clientId(người gửi)
            emit newImageData(dataImage,clientID);
        }
        else if (dataImage != QString() && clientID != QString() && type == "drawData")
        {
            qDebug() << "Mess5:  New DrawImage";
            emit newDrawData(dataImage,clientID );
        }
    }
    // 7. type:castVote;payLoad:clientID;sender:1111
    else if (separated.first() == "type:castVote")
    {
        qDebug() << "Mess 7. Server App: CastVote";
        separated.pop_front();
        QString voteImage = QString(); // Lưu trữ tên bức ảnh do client bình chọn ( VD : 6543)
        QString senderId = QString(); // người bình chọn (-  clientId)
        if(separated.front().contains("payLoad:"))
        {
            voteImage = separated.front();
            voteImage = voteImage.remove("payLoad:");
        }
        separated.pop_front();
        if(separated.front().contains("sender:"))
        {
            senderId = separated.front();
            senderId = senderId.remove("sender:");
        }

        if(voteImage != QString() && senderId != QString())
        {
            emit newVote(voteImage,senderId);
        }
    }

}
