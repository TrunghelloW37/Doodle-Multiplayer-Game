#include "GameManager.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QByteArray>

GameManager::GameManager(QObject *parent)
    : QObject{parent},
    m_roomLobbyCode(QString()),
    m_clientId(QString()),
    m_clientsInLobby(QStringList()),
    m_readyClientList(QStringList()),
    m_drawInstruction(QString()),
    m_isDrawing(false),
    m_drawList(QStringList()),
    m_winner(QString()),
    m_voteFinished(false)
{
    m_messageHandler = new MessageProcessHandler(this);
    // I. Signal(MessageProcessHandler) - Slot(GameManager)
    connect(m_messageHandler,&MessageProcessHandler::uniqueIdRegistration, this,&GameManager::registerUniqueID);
    // 1. Phát ra tín hiệu thêm người chơi vào phòng (create game + joinGame)
    connect(m_messageHandler,&MessageProcessHandler::newLobby, this,&GameManager::lobbyJoined);
    // 2. Phát ra tín hiệu khi có người chơi tham gia vào phòng thì phát ra tín hiệu để cập nhật các người chơi trong phòn
    connect(m_messageHandler,MessageProcessHandler::lobbyListUpdated,this,&GameManager::setClientsInLobby);
    // 3. Tín hiệu newLobbyMessage sẽ gửi message sang QML (Signal - Signal)
    connect(m_messageHandler,&MessageProcessHandler::newLobbyMessage,this,&GameManager::newLobbyMessage);
    // 4. Phát ra tín hiệu khi client Ready
    connect(m_messageHandler,&MessageProcessHandler::readyListChanged,this,&GameManager::newClientReadyList);
    // 5. Tín hiệu này phát ra khi tất cả các client đều Ready (Signal - Signal)
    connect(m_messageHandler,&MessageProcessHandler::gameStarted,this,&GameManager::gameStarting);
    // 6. Phát ra tín hiệu này khi có dữ liệu hình vẽ và chủ đề để client vẽ
    connect(m_messageHandler,&MessageProcessHandler::dataImageAndInstructuon,this,&GameManager::dataImageAndInstructionReady);
    // 7. Phát ra tín hiệu khi tất cả người chơi đã có ảnh hoàn chỉnh
    connect(m_messageHandler,&MessageProcessHandler::gameDrawAllReady,this,&GameManager::gameDrawingsReady);
    // 8. Phát ra tín hiệu đã có người có lượt bình chọn cao nhất
    connect(m_messageHandler,&MessageProcessHandler::winnerChosen,this,&GameManager::winnerChosen);


}

GameManager::~GameManager()
{
    m_messageHandler->deleteLater();
}

// Lấy Id của client
QString GameManager::myId()
{
    return m_clientId;
}


// 1. Hàm gửi yêu cầu tạo Create game
void GameManager::createGameRequest()
{
    qDebug() <<"Client APP: Create Game Request";
    // type:createGame;payLoad:0;sender:5555
    emit newMessageReadyToSend("type:createGame;payLoad:0;sender:"+m_clientId ); // connect tại main.cpp
}

// 2. Hàm gửi yêu cầu join Room
void GameManager::joinLobbyRequest(QString lobbyId)
{
    qDebug() << "Client APP: new Client join Lobby Room";
    // m_roomLobbyCode : Id phòng  --- m_clienId :  clientId
    emit newMessageReadyToSend("type:joinGame;payLoad:"+lobbyId+";sender:"+m_clientId);
}

// 3. Hàm gửi đoạn chat tới các Client ( message được lấy từ TextFiled QML)
void GameManager::sendMessageToLobby(QString message)
{
    qDebug() <<"Client App: Message Request: " << message;
    // type:messageGame; payLoad:"helloWordl";LobbyId:5999;sender:5000
    emit newMessageReadyToSend("type:messageGame;payLoad:" + message+ ";lobbyID:" + m_roomLobbyCode + ";sender:" + m_clientId);
}

// 4. Hàm gửi yêu cầu ready
void GameManager::readyToPlay(QString checkReady)
{
    qDebug()<< "Client App: Client Ready To Play: " << checkReady;
    // type:readyToPlay;payLoad:1,sender:1111
    emit newMessageReadyToSend("type:readyToPlay;payLoad:" + checkReady + ";sender:" + m_clientId);
}

// 5. Kiểm tra client Đã sẵn sàng hay chưa?
bool GameManager::isClientReady(QString clientID)
{
    return m_readyClientList.contains(clientID);
}

// 6. Hàm gửi Data ảnh và client Id xuống Server ( Hàm này được gọi bên QML WaitingForPlayrsScreen.qml)
void GameManager::drawDone()
{
    if(m_voteFinished == true)
    {
        return;
    }
    // Đường dẫn file ảnh vừa vẽ nằm tại
    QFile imageFile("D:\\build-Game-Desktop_Qt_5_15_2_MinGW_64_bit-Debug\\Client_Game\\tmp.png");
    if(!imageFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open File Image";
        return;
    }
    QByteArray dataImage = imageFile.readAll();
    imageFile.close();

    QString dataPacket = QString();

    // Kiểm tra xem ảnh đã vẽ xong - hoàn tất hay chưa ?
    if(m_isDrawing == true)
    {
        qDebug() << "drawData : m_isDrawing: " << m_isDrawing;
        //  ảnh của client Sau khi đã vẽ chủ đề xong.
        // type:drawData;payLoad:dataImage;sender:clientId
        dataPacket = "type:drawData;payLoad:" + dataImage.toHex() +";sender:" + m_clientId;
    }
    else
    {
        // Ảnh vẽ lúc ban đầu
        // type:imageData;payLoad:dataImage;sender:clientId
        dataPacket = "type:imageData;payLoad:" + dataImage.toHex() +";sender:" + m_clientId;
    }
    qDebug() << "DataPacket: " << dataPacket;
    emit newMessageReadyToSend(dataPacket);
}

// 7. phương thứ này để load ảnh bên QML
QString GameManager::imageFilePath()
{
    qDebug() << "Client App : Had call ImageFilePath(): ";
    QString localPath = QDir::currentPath();
    QString ret = "file:///" + localPath + QDir::separator() + m_clientId + ".png";
    qDebug() <<"Path: " << ret;
    return ret;

}

// 8. Hàm này chọn ảnh vẽ đẹp nhất ( Hàm được gọi bên QML : voidScreen.qml)
// imageURL : file:///D:\build-Game-Desktop_Qt_5_15_2_MinGW_64_bit-Debug\Client_Game\temp\4827.png
void GameManager::castVote(QString imageURL)
{
    // Đã bình chọn xong;
    m_voteFinished = true; // DrawDone
    QFileInfo fileInfo(imageURL);
    // type:castVote;payLoad:clientID;sender:1111
    emit newMessageReadyToSend("type:castVote;payLoad:" + fileInfo.baseName() +";sender:" + m_clientId);

}

QString GameManager::winnerImage()
{
    QDir workingDir("D:\\build-Game-Desktop_Qt_5_15_2_MinGW_64_bit-Debug\\Client_Game");
    workingDir.mkdir("temp"); // Tạo thư mục temp trong đường dẫn

    // Đường dẫn đến bức ảnh đẹp nhất
    QString fileImageBest = "D:\\build-Game-Desktop_Qt_5_15_2_MinGW_64_bit-Debug\\Client_Game\\temp\\"  + m_winner + ".png";
    fileImageBest.prepend("file:///");
    return fileImageBest;
}


// Slots
//  Xử lý tin nhắn từ Server gửi đến
void GameManager::processSocketMessage(QString message)
{
    // process Massage : Xử lý tin nhắn
    m_messageHandler->processMessage(message);
}

// Hàm Đăng kí Client Id
void GameManager::registerUniqueID(QString uniqueId)
{
    // registering : Đăng kí  || unique : duy nhất
    qDebug() << "Client App: ClientId is: " + uniqueId;
    m_clientId = uniqueId;
}

// Hàm thiết lập Id phòng và các người chơi trong phòng
void GameManager::lobbyJoined(QString lobbyId , QStringList clients)
{
    setRoomLobbyCode(lobbyId); // QString : Id phòng
    setClientsInLobby(clients); // QStringList --> chưa các Id client
    qDebug() <<"Client App : lobbyJoined() ---> RoomId: " << lobbyId <<" - " <<"ClientId: "<< clients;
    emit inGameLobby(); // -> Phát tín hiệu sang QML để chuyển sang trang (LobbyScreen.qml)
}

// Hàm thiết lại trạng thái của các người chơi trong QStringList m_readyClientList
void GameManager::newClientReadyList(QStringList readyClients)
{
    if(m_readyClientList != readyClients)
    {
        m_readyClientList = readyClients;
        emit readyClientListChanged(); // connect LobbyScreen.qml (Image check)
    }
}


// Khi tất cả người chơi đã có dữ liệu hình ảnh và chủ đề cho người chơi để vẽ ( ảnh ban đầu)
void GameManager::dataImageAndInstructionReady(QString data, QString instruction)
{
    qDebug() << "Client App: Set Write Image Client: " << instruction;
    setDrawInstruction(instruction); // Thiết lập chủ đề vẽ

    m_isDrawing = true;

    // Khởi tạo file (kèm đường dẫn)
    // dữ liệu hình ảnh vẽ được ghi vào file
    QFile tempImage("D:\\build-Game-Desktop_Qt_5_15_2_MinGW_64_bit-Debug\\Client_Game\\" +  m_clientId + ".png");
    if(!tempImage.open(QIODevice::WriteOnly))
    {
        qDebug() << "Client App: Cannot Write file";
        return;
    }

    // CHuyển đối data từ kiểu hex sang QByteArray
    QByteArray fileContents = QByteArray::fromHex(data.toLocal8Bit());
    tempImage.write(fileContents); // --> Ghi dữ liệu ảnh vào file
    tempImage.flush();
    tempImage.close();
    //
    emit drawInstructionTime(); // Connection QML ( main.qml)
    qDebug() <<"Client App: Had Send emit drawInstructionTime";
}

// Hàm này tạo đường dẫn cho file ảnh và lưu tất cả cả đường dẫn ảnh vào QStringList
// Từ QML có thể truy cập vào QStringList này để lấy tất cả các ảnh của client
void GameManager::gameDrawingsReady(QStringList dataImages, QStringList clients)
{
    //Lấy đường dẫn đến thư mục hiện tại
    QDir workingDir("D:\\build-Game-Desktop_Qt_5_15_2_MinGW_64_bit-Debug\\Client_Game");
    workingDir.mkdir("temp"); // Tạo thư mục temp trong đường dẫn ( để lưu trữ các ảnh)

    QString filePath = "D:\\build-Game-Desktop_Qt_5_15_2_MinGW_64_bit-Debug\\Client_Game\\temp\\";
    QStringList newFilesImage; // Dùng để chứa các đường ảnh các file ảnh
    for (int i = 0 ; i < clients.size(); i++)
    {
        // / Tạo từng file ảnh theo tên clientId trong đường dẫn ( Mỗi file ảnh tạo ra có clientId tương ứng)
        QString fileName = filePath + clients.at(i) + ".png";
        QFile tempFile(fileName);
        if(!tempFile.open(QIODevice::WriteOnly))
        {
            qDebug() <<"gameDrawingsReady : Open File False";
        }
        QByteArray dataImage = QByteArray::fromHex(dataImages.at(i).toLocal8Bit());
        tempFile.write(dataImage);
        tempFile.flush();
        tempFile.close();

        // prepend để ghép vào trước chuỗi
        fileName.prepend("file:///");
        // Thêm vào QStringList ( dùng để chứa đường ảnh file ảnh của tất cả client)
        newFilesImage.append(fileName);
    }
    setDrawList(newFilesImage);
    emit votingTime(); // Gửi tín hiệu QML bắt đầu bình chọn ( connnect main.qml)
}

//  Hàm này chọn người có lượt bình chọn nhiều nhất
void GameManager::winnerChosen(QString winner)
{
    setWinner(winner);
    emit gameOver();
    qDebug()<<"CLient app: Had emit GameOver!!";
}


// Getter - Setter
QString GameManager::roomLobbyCode() const
{
    return m_roomLobbyCode;
}

QStringList GameManager::clientsInLobby() const
{
    return m_clientsInLobby;
}


void GameManager::setRoomLobbyCode(const QString &newRoomLobbyCode)
{
    if (m_roomLobbyCode == newRoomLobbyCode)
        return;
    m_roomLobbyCode = newRoomLobbyCode;
    emit roomLobbyCodeChanged();
}

void GameManager::setClientsInLobby(const QStringList &newClientsInLobby)
{
    if (m_clientsInLobby == newClientsInLobby)
        return;
    m_clientsInLobby = newClientsInLobby;
    emit clientsInLobbyChanged();
}

//
QString GameManager::drawInstruction() const
{
    return m_drawInstruction;
}

void GameManager::setDrawInstruction(const QString &newDrawInstruction)
{
    if (m_drawInstruction == newDrawInstruction)
        return;
    m_drawInstruction = newDrawInstruction;
    emit drawInstructionChanged();
}

//
QStringList GameManager::drawList() const
{
    return m_drawList;
}

void GameManager::setDrawList(const QStringList &newDrawList)
{
    if (m_drawList == newDrawList)
        return;
    m_drawList = newDrawList;
    emit drawListChanged();
}

QString GameManager::winner() const
{
    return m_winner;
}

void GameManager::setWinner(const QString &newWinner)
{
    if (m_winner == newWinner)
        return;
    m_winner = newWinner;
    emit winnerChanged();
}
