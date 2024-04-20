#include "GameLobbyHandler.h"
#include <QDebug>
#include <random>
#include <QDateTime>

// Game Lobby : Sảnh trò chơi
GameLobbyHandler::GameLobbyHandler(QString lobbyId, QObject *parent)
    : QObject{parent}
{
    m_LobbyId = lobbyId;
    m_clientReadyList.clear();
    m_clientDataImage.clear();
    m_clientDrawImage.clear();
    // Chủ đề để vẽ hình
    m_objectsToDraw = QStringList() << "dog" << "cat" << "elephant" << "car" << "ball";
}

void GameLobbyHandler::addClient(QString clientId)
{
    if(!m_gameClientList.contains(clientId))
    {
        m_gameClientList.append(clientId);
    }

    m_clientReadyList.clear();
    // QMap<QString,bool> :
    // + clientId  ---  bool : Trạng thái
    for(const QString& client : m_gameClientList)
    {
        m_clientReadyList[client] = false;
    }
}

// Trả về chuỗi các Id Client có trong QList<QString> m_gameClientList;
QString GameLobbyHandler::clientsInLobby()
{
    QString tmp = "";
    for(const QString& client : m_gameClientList)
    {
        tmp += client + ",";
    }
    tmp.chop(1); // loại bỏ 1 kí tự cuỗi chuỗi ( ở đây bỏ dấu ",")
    qDebug() << "Server App : clientInLobby: " << tmp;

    return tmp;
}

QStringList GameLobbyHandler::clientsInLobbyList()
{
    return m_gameClientList;
}


// Client sẵn sàng để chơi
// Hàm khi người dùng bấm nút Ready thì nó kiểm tra lun tất cả các client trong QMap để ready hết chưa
void GameLobbyHandler::userReadyToPlay(QString checkReady , QString clientId)
{
    if(m_gameClientList.contains(clientId))
    {
        m_clientReadyList[clientId] = (checkReady == "true" ? true : false);
        emit userReadyListChanged();

        bool notReady = false; // Nếu client all Ready --> notReady = false
        for(const QString& clientId : m_clientReadyList.keys())
        {
            if(!m_clientReadyList[clientId])
            {
                notReady = true; // Một số client chưa sẵn sàng
            }
        }

        // Nếu tất cả người chơi đã sẵn sàng
        if(notReady == false)
        {
            emit gameReadyToBegin();
        }
    }
}

// Hàm trả về các client đã sẵn sàng để vào game
QString GameLobbyHandler::whoIsReady()
{
    QString returnValue = QString();
    QStringList clientsInReadyList = m_clientReadyList.keys(); // --> 1111,6789,4444
    for(int i=0; i < clientsInReadyList.size(); i++)
    {
        QString clientId = clientsInReadyList[i]; // Trả về các ClientId
        if(m_clientReadyList[clientId] == true)
        {
            returnValue.append(clientId + ",");
        }
    }

    if(returnValue != QString())
    {
        returnValue.chop(1);
    }
    qDebug() << "Server App : Who is Ready : " << returnValue;
    return returnValue;
}

// Hàm thiết lập clientId - DataImage (lúc mới bắt đầu vẽ chưa có đề tài)
void GameLobbyHandler::newImageData(QString imageData, QString clientId)
{
    qDebug() <<"Server App: Game LobbyHandler New ImageData";
    // Kiểm tra xem trong Lobby này có clientId ko ??
    if(m_gameClientList.contains(clientId))
    {
        // QMap<ClientId , imageData>
        m_clientDataImage[clientId] = imageData;
        qDebug() <<"ClientDataImage Size:  "<< m_clientDataImage.size();
        if(m_clientDataImage.keys().size() == m_gameClientList.size())
        {
            // key = clientId , value = dataImage
            QMap<QString,QString> distributedImage; // distributed : Phân phối
            for(int index = 0; index < m_gameClientList.size(); index++)
            {
                QString currentIndex = m_gameClientList.at(index);
                QString doodle; // doodle : dữ liệu hình ảnh
                if(index == m_gameClientList.size()-1)
                {
                    doodle = m_clientDataImage[m_gameClientList.at(0)];
                }
                else
                {
                    doodle = m_clientDataImage[m_gameClientList.at(index+1)];
                }
                distributedImage[currentIndex] = doodle;
                // Phân phối ảnh cho các client
                /*
                 * [1111,dataImage1111]
                 * [2222,dataImage2222]
                 * [3333,dataImage3333]
                 * |
                 * [1111,dataImage2222]
                 * [2222,dataImage3333]
                 * [3333,dataImage1111]
                 * */
            }
            choosenObject(); // Chọn chủ vẽ ngẫu nhiên
            // Gửi tín hiệu tất cả client đã có ảnh QMap<clientId,ImageData)
            emit allDataImageReceived(distributedImage);
            qDebug() <<"Server App : Had emit add Data Image";
        }
    }
}

// Hàm thiết lập clientId - DataImage (lúc vẽ xong)
void GameLobbyHandler::newDrawData(QString imageData, QString clientId)
{
    qDebug() <<"Server App: New DrawData";
    // Kiểm tra xem trong Lobby này có clientId ko ??
    if(m_gameClientList.contains(clientId))
    {
        // QMap<ClientId , imageData>
        m_clientDrawImage[clientId] = imageData;
        /*
          [1111,dataImage1111]
          [2222,dataImage2222]
          [3333,dataImage3333]
        */
        if(m_clientDrawImage.keys().size() == m_gameClientList.size())
        {
            emit allDataDrawReceived(m_clientDrawImage);
            qDebug() <<"Server App : Had emit add Draw Data ";
        }
    }
}

// Hàm Ngẫu nhiên Chọn chủ đề
void GameLobbyHandler::choosenObject()
{
    // gẫu nhiên Chọn chủ đề tài vễ
    std::default_random_engine generator;
    generator.seed(QDateTime::currentMSecsSinceEpoch());
    std::uniform_int_distribution<int> objectGenerator(0,m_objectsToDraw.size());
    // Tự động lựa chủ đề
    m_selectedObject = m_objectsToDraw[objectGenerator(generator)];

}

QString GameLobbyHandler::drawInstruction()
{
    return m_selectedObject;
}

// Hàm bình chọn ảnh
void GameLobbyHandler::newVote(QString voteImage, QString clientId)
{
    qDebug() <<"Server App: New Vote: " << voteImage << " - " << clientId;
    if(m_gameClientList.contains(clientId) && !m_clientsWhoVoted.contains(clientId))
    {
        // QMap<QString,int> m_voteResult
        // + QString : tên bức ảnh --- int : cho lần bình chọn
        // Kiểm tra xem bức ảnh đã có ai bình chọn từ trước chưa ?
        if(m_voteResult.contains(voteImage))
        {
            m_voteResult[voteImage]++;
        }
        // nếu chưa - ( có nghĩa đây là người đầu tiên bình chọn bức ảnh)
        else
        {
            m_voteResult[voteImage] = 1;
        }
        m_clientsWhoVoted.append(clientId);
        // Kiểm tra xem tất cả người chưa có vote(bình chọn) hết hay chưa ??
        if(m_clientsWhoVoted.size() == m_gameClientList.size())
        {
            QString winner = QString();
            int highestVote = 0; // lượt chọn cao nhất
            QList<QString> imagesVote = m_voteResult.keys();
            for(const QString& image : imagesVote)
            {
                if(m_voteResult[image] > highestVote)
                {
                    highestVote = m_voteResult[image];
                     // tên ảnh(image) cũng chính là id client có lượt vote ảnh cao nhất
                    winner = image;
                }
            }
            emit winnerChosen(winner);
        }
    }
}
