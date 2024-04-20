#include "GameManager.h"
#include <random>
#include <QDateTime>
#include <QMap>

GameManager::GameManager(QObject *parent)
    : QObject{parent}
{
    m_socketHandler = new WebSocketHandler(this);
    m_messageProcessHandler = new MessageProcessorHandler(this);
    // connect - Xử lý tin nhắn
    connect(m_socketHandler,&WebSocketHandler::newMessageToProcess,
            m_messageProcessHandler,&MessageProcessorHandler::processMessage);
    // connect createrGame ( tín hiệu gửi Id client muốn Create Game)
    connect(m_messageProcessHandler,&MessageProcessorHandler::createGameRequest,
            this,&GameManager::createGameLobbyRequest);
    // connect JoinGame ( tín hiệu gửi 2 giá trị : lobbyId : Id phòng và senderId : id client muốn join)
    connect(m_messageProcessHandler,&MessageProcessorHandler::joinGameLobbyRequest,
            this,&GameManager::jointGameLobbyRequest);
    // connect send Message To all Clients
    connect(m_messageProcessHandler,&MessageProcessorHandler::messageLobbyToRequest,
            this,&GameManager::messageLobbyRequets);
    // connect tín hiệu Ready
    connect(m_messageProcessHandler,&MessageProcessorHandler::clientReadyToPlay,
            this,&GameManager::userReadyToPlay);
    // Hàm này phát ra tín hiệu khi Server nhận được dữ liệu hình ảnh (imageData) từ client + clientId
    connect(m_messageProcessHandler,&MessageProcessorHandler::newImageData,
            this,&GameManager::newImageDataReady);
    connect(m_messageProcessHandler,&MessageProcessorHandler::newDrawData,
            this,&GameManager::newDrawDataReady);
    // Vote
    connect(m_messageProcessHandler,&MessageProcessorHandler::newVote,
            this,&GameManager::newVote);

}

// Điều này đảm bảo rằng đối tượng sẽ không bị hủy trong khi nó vẫn đang được sử dụng.
GameManager::~GameManager()
{
    m_socketHandler->deleteLater();
}

// Hàm CreateGame (uniqueId là Id người muốn tạo phòng)
void GameManager::createGameLobbyRequest(QString uniqueId)
{
    // Tạo số Id phòng ngẫu nhiên từ 1000 - 9999
    std::default_random_engine generator;
    generator.seed(QDateTime::currentMSecsSinceEpoch());
    std::uniform_int_distribution<int> idGenerator(1000,9999);

    // newLobbyId là Id Phòng  ---  Tạo Ngẫu nhiên Id Phòng (newLobbyId)
    QString newLobbyId = QString::number(idGenerator(generator));
    while(m_gameLobbyMap.keys().contains(newLobbyId))
    {
        newLobbyId = QString::number(idGenerator(generator));
    }

    // Đối tượng newGameLobby dùng để chứa Id phòng và các người chơi tham gia vào phòng
    GameLobbyHandler* newGameLobby = new GameLobbyHandler(newLobbyId,this);

    // 1. tín hiệu này được phát ra khi thay đổi trạng ( false --> true)
    connect(newGameLobby,&GameLobbyHandler::userReadyListChanged,this,&GameManager::userReadyListChanged);
    // 2. Tín hiệu này được phát ra khi tất cả các client đã sẵn sàng
    connect(newGameLobby,&GameLobbyHandler::gameReadyToBegin,this,&GameManager::gameLobbyReadyToBegin);
    // 3. Tín hiệu này được phát ra khi tất cả client đã có dữ liệu hình ảnh(dataImage - ảnh ban đầu)
    connect(newGameLobby,&GameLobbyHandler::allDataImageReceived,this,&GameManager::allImagesInGameLobbyReceived);
    // 4. Tín hiệu này được phát ra khi tất cả client đã có dữ liệu hình ảnh(dataImage - ảnh đã hoàn thành)
    connect(newGameLobby,&GameLobbyHandler::allDataDrawReceived,this,&GameManager::allDrawInGameLobbyReceived);
    // 5. Tín hiệu này được phát ra khi chọn được bức ảnh có nhiều lượt vote nhất
    connect(newGameLobby,&GameLobbyHandler::winnerChosen,this,&GameManager::winerChoosen);

    // Thêm người tạo game vào Phòng
    newGameLobby->addClient(uniqueId);
    // QMap<QString,GameLobbyHandler*> m_gameLobbyMap
    m_gameLobbyMap[newLobbyId] = newGameLobby;
    qDebug() <<"1. Server: CreaterGame by ClientId:" << uniqueId <<"- Id Room: " << newLobbyId;
    // newLobbyId : Id phòng  - uniqueId : clientId tạo phòng
    m_socketHandler->sendTextMessageToClient("type:newLobbyCreated;payLoad:"+newLobbyId
                                          +";clientList:" + newGameLobby->clientsInLobby(),uniqueId);
}

// Hàm Client tham gia vào phòng với lobbyId là tên phòng , uniqueId là Id client muốn tham gia vào phòng
void GameManager::jointGameLobbyRequest(QString lobbyId, QString uniqueId)
{
    qDebug() << "2. Server: Client -"<< uniqueId<<"- Join LobbyId:" << lobbyId;
    // Kiểm tra xem trong QMap<QString,GameLobbyHandler*> đã có Id phòng(lobbyId) chưa ?
    if(m_gameLobbyMap.contains(lobbyId))
    {
        GameLobbyHandler* existingLobby = m_gameLobbyMap[lobbyId];
        existingLobby->addClient(uniqueId);
        // cập nhật cho client mới tham gia vào list các clients trước đó
        m_socketHandler->sendTextMessageToClient("type:joinSuccess;payLoad:"+lobbyId +
                                        ";clientList:" + existingLobby->clientsInLobby(),uniqueId);

        // cập nhật lại người chơi cho tất cả các client đã tham gia vào phòng
        m_socketHandler->sendTextMessageToMultiClients("type:updatedClientList;payLoad:" + existingLobby->clientsInLobby(),
                                                       existingLobby->clientsInLobbyList());
    }
    else
    {
        // Gửi tín hiệu tham gia phòng thất bại cho client
        m_socketHandler->sendTextMessageToClient("type:joinError;payLoad:DNE",uniqueId);
    }
}

// Hàm nhắn tin giữa các clients
void GameManager::messageLobbyRequets(QString message, QString lobbyId, QString senderId)
{
    if(m_gameLobbyMap.contains(lobbyId))
    {
        GameLobbyHandler* existingLobby = m_gameLobbyMap[lobbyId];
        // Nói gửi message + id client(người gửi tin nhắn) đến tất cả các client khác
        m_socketHandler->sendTextMessageToMultiClients("type:lobbyMessage;payLoad:"+message
                                                           + ";sender:"+senderId, existingLobby->clientsInLobbyList());
    }
}

// Hàm Gửi cho tất cả client danh sách các client đã ready
void GameManager::userReadyListChanged()
{
    // sender() trả về đối tượng đã phát ra tín hiệu cho kết nối hiện tại
    GameLobbyHandler* existingLobby = qobject_cast<GameLobbyHandler*>(sender());
    m_socketHandler->sendTextMessageToMultiClients("type:readyListChanged;payLoad:" + existingLobby->whoIsReady(),
                                                   existingLobby->clientsInLobbyList());
    qDebug() << "4. Server: Client had ready: " + existingLobby->whoIsReady();

}

// uniqueID là Id client gửi
// Hàm này sửa lại trạng thái của client ( false --> true)
// Khi 1 client vào phòng thì trạng thái của client == false
void GameManager::userReadyToPlay(QString checkReady , QString uniqueID)
{
    QList<GameLobbyHandler*> gameLobbyList = m_gameLobbyMap.values();
    for(GameLobbyHandler* existingLobby : gameLobbyList)
    {
        existingLobby->userReadyToPlay(checkReady,uniqueID);
    }
}

// Hàm phát ra tín hiệu khi tất cả các client đã sẵn sàng
void GameManager::gameLobbyReadyToBegin()
{
    qDebug() <<"6. Server: All Client Are Ready --> gameReadyToBegin";
    // sender() trả về đối tượng đã phát ra tín hiệu cho kết nối hiện tại
    GameLobbyHandler* existingLobby = qobject_cast<GameLobbyHandler*>(sender());
    // type:gameReadyToBegin;payLoad:0
    m_socketHandler->sendTextMessageToMultiClients("type:gameReadyToBegin;payLoad:0",
                                                   existingLobby->clientsInLobbyList());
}


// Khi có dữ liệu ảnh mới từ Client
// + dataImage : dữ liệu hex của hình ảnh
// + client Id :  Id  của người gửi ảnh
void GameManager::newImageDataReady(QString dataImage, QString clientId)
{
    qDebug() <<"7. Server: Client ---> ImageData";
    QList<GameLobbyHandler*> gameLobbyList = m_gameLobbyMap.values();
    for(GameLobbyHandler* existingLobby : gameLobbyList)
    {
        existingLobby->newImageData(dataImage , clientId);
    }
}

void GameManager::newDrawDataReady(QString dataImage, QString clientId)
{
    qDebug() <<"7. Server: Client ---> DrawImage";
    QList<GameLobbyHandler*> gameLobbyList = m_gameLobbyMap.values();
    for(GameLobbyHandler* existingLobby : gameLobbyList)
    {
        existingLobby->newDrawData(dataImage , clientId);
    }
}

//distributedMap : QMap <clientId,dataImage>
void GameManager::allImagesInGameLobbyReceived(QMap <QString,QString> distributedMap)
{
    qDebug() <<"8. Server: All Client ---> ImageData";
    // sender() trả về đối tượng đã phát ra tín hiệu cho kết nối hiện tại
    GameLobbyHandler* existingLobby = qobject_cast<GameLobbyHandler*>(sender());
    // type:drawingInstruction;payLoad:imageData;instruction:dog  (instruction :  chủ đề)
    for(const QString& client : distributedMap.keys())
    {
        // distributedMap[client] : data hình ảnh của client
        // existingLobby->drawInstruction(); : chủ đề hình vẽ ( được lựa chọn ngãu nhiên)
        QString dataPacket = "type:drawingInstruction;payLoad:" + distributedMap[client]
                             +";instruction:" + existingLobby->drawInstruction();
        m_socketHandler->sendTextMessageToClient(dataPacket,client);
    }

}

//distributedMap : QMap <clientId,dataImage>
// type:gameImages;payLoad:dataImage1,dataImage2,dataImage3;clients:1111,2222,3333
// --> Hàm này gửi dữ liệu của tất cả các ảnh đến từng Client
void GameManager::allDrawInGameLobbyReceived(QMap<QString, QString> drawingMap)
{
    qDebug() << "Server App: Had call allDrawInGameLobbyReceived()";
    // sender() : Trả về đối tượng đã phát ra tín hiệu này
    GameLobbyHandler* existingLobby = qobject_cast<GameLobbyHandler*>(sender());
    QString ret = "type:gameImages;payLoad:";
    QString payLoad = QString(); //  chứa Dữ liệu của tất cả các ảnh
    QString clients = QString(); // chứa Id của tất cả các client

    for(QMap<QString,QString>::iterator it = drawingMap.begin();
         it != drawingMap.end() ; it++)
    {
        clients.append(it.key() + ",");
        payLoad.append(it.value() + ",");
    }

    clients.chop(1);
    payLoad.chop(1);

    // Thêm vào cuối ret
    ret.append(payLoad + ";clients:" + clients);

    // type:gameImages;payLoad:dataImage1,dataImage2,dataImage3;clients:1111,2222,3333
    // phương thức này gửi dữ liệu tất cả các ảnh đến từng Client
    m_socketHandler->sendTextMessageToMultiClients(ret,existingLobby->clientsInLobbyList());

}

// Hàm này xử lý việc vote các bức ảnh của các client để chọn ra client có nhiều vote nhất
// + voteImage : bức ảnh được chọn
// + clientId : người chọn ảnh
void GameManager::newVote(QString voteImage, QString clientId)
{
    // sender() trả về đối tượng đã phát ra tín hiệu cho kết nối hiện tại
    QList<GameLobbyHandler*> gameLobbyList = m_gameLobbyMap.values();
    for(GameLobbyHandler* existingLobby : gameLobbyList)
    {
        existingLobby->newVote(voteImage,clientId);
    }
}

//
void GameManager::winerChoosen(QString winner)
{
    GameLobbyHandler* existingLobby = qobject_cast<GameLobbyHandler*>(sender());
    // type:winnerChosen;payLoad:winner
    m_socketHandler->sendTextMessageToMultiClients("type:winnerChosen;payLoad:" + winner , existingLobby->clientsInLobbyList());
}
