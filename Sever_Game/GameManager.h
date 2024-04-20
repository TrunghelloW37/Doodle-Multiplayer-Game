#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "websockethandler.h"
#include "MessageProcessorHandler.h"
#include "GameLobbyHandler.h"

class GameManager : public QObject
{
    Q_OBJECT
public:
    explicit GameManager(QObject *parent = nullptr);
    ~GameManager();

signals:

public slots:
    // Các hàm thực hiện chứa năng sau khi được phân tích các yêu cầu được gửi đến từ Client.
    void createGameLobbyRequest(QString uniqueId);
    void jointGameLobbyRequest(QString lobbyId , QString uniqueId);
    void messageLobbyRequets(QString message , QString lobbyId , QString senderId);
    void userReadyListChanged();
    void userReadyToPlay(QString checkReady ,QString uniqueID);
    void gameLobbyReadyToBegin();
    //
    void newImageDataReady(QString dataImage , QString clientId);
    void newDrawDataReady(QString dataImage , QString clientId);
    void allImagesInGameLobbyReceived(QMap<QString,QString> distributedMap);
    void allDrawInGameLobbyReceived(QMap<QString,QString> drawingMap);
    void newVote(QString voteImage , QString clientId);
    void winerChoosen(QString winner);
private:
    WebSocketHandler* m_socketHandler;
    MessageProcessorHandler* m_messageProcessHandler;
    QMap<QString,GameLobbyHandler*> m_gameLobbyMap;

};

#endif // GAMEMANAGER_H
