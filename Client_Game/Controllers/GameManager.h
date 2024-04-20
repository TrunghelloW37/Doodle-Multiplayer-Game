#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "MessageProcessHandler.h"

class GameManager : public QObject
{
    Q_OBJECT
    // Id Room
    Q_PROPERTY(QString roomLobbyCode READ roomLobbyCode WRITE setRoomLobbyCode NOTIFY roomLobbyCodeChanged)
    // Model ListView ( Hiện thị các Client tham gia)
    Q_PROPERTY(QStringList clientsInLobby READ clientsInLobby WRITE setClientsInLobby NOTIFY clientsInLobbyChanged)
    // Chủ đề tài vẽ
    Q_PROPERTY(QString drawInstruction READ drawInstruction WRITE setDrawInstruction NOTIFY drawInstructionChanged)
    // Chứa tất cả các đường dẫn file ảnh của client
    Q_PROPERTY(QStringList drawList READ drawList WRITE setDrawList NOTIFY drawListChanged)
    // Chứa người chơi có ảnh nhiều lượt vote nhất
    Q_PROPERTY(QString winner READ winner WRITE setWinner NOTIFY winnerChanged)
public:
    explicit GameManager(QObject *parent = nullptr);
    ~GameManager();
    // getter
    QString roomLobbyCode() const;
    QStringList clientsInLobby() const;
    QString drawInstruction() const;
    QStringList drawList() const;
    QString winner() const;

    // Các chứa năng được gọi bên QML
    Q_INVOKABLE QString myId();
    Q_INVOKABLE void createGameRequest();
    Q_INVOKABLE void joinLobbyRequest(QString lobbyId);
    Q_INVOKABLE void sendMessageToLobby(QString message);
    Q_INVOKABLE void readyToPlay(QString checkReady);
    Q_INVOKABLE bool isClientReady(QString clientID);
    Q_INVOKABLE void drawDone();
    Q_INVOKABLE QString imageFilePath();
    Q_INVOKABLE void castVote(QString imageURL);
    Q_INVOKABLE QString winnerImage();



public slots:
    // Setter
    void setRoomLobbyCode(const QString &newRoomLobbyCode);
    void setClientsInLobby(const QStringList &newClientsInLobby);
    void setDrawInstruction(const QString &newDrawInstruction);
    void setDrawList(const QStringList &newDrawList);
    void setWinner(const QString &newWinner);
    //
    void processSocketMessage(QString message); // Xử lý tin nhắn
    void registerUniqueID(QString uniqueId); // Đăng kí Id cho client
    void lobbyJoined(QString lobbyId , QStringList clients); // Client Tham gia vào phòng
    void newClientReadyList(QStringList readyClients); // Kiểm tra trạng thái của các client
    // Khi tất cả người chơi đã có dữ liệu hình ảnh ( ảnh chưa Hoàn thiện) và chủ đề cho người chơi để vẽ
    void dataImageAndInstructionReady(QString data , QString instruction);
    // Khi tất cả người chơi đã có tất dữ liệu hình ảnh của các người chơi khác(ảnh Hoàn thiện)
    void gameDrawingsReady(QStringList dataImages, QStringList clients);
    void winnerChosen(QString winner);

signals:
    // các tín hiệu này connect tại main.cpp
    // Changed Q_Property
    void roomLobbyCodeChanged();
    void clientsInLobbyChanged();
    void drawInstructionChanged();
    void drawListChanged();
    void winnerChanged();
    //
    void newMessageReadyToSend(QString message);
    void inGameLobby();
    void newLobbyMessage(QString message); // Hàm gửi message cho các client
    void readyClientListChanged();
    void drawInstructionTime();
    void gameStarting(); // Connect main.qml
    void votingTime();
    void gameOver();


private:
    QString m_clientId; // Clients Id
    QString m_roomLobbyCode; // Id Phòng
    QStringList m_clientsInLobby; //  người chơi cùng phòng
    QStringList m_readyClientList; // QString chứa các người chơi đã sẵn sàng
    MessageProcessHandler *m_messageHandler; // Xử lý tin nhắn gửi đến
    QString m_drawInstruction; // đề tài của hình vẽ
    bool m_isDrawing;
    QStringList m_drawList; // DÙng để chứa tất cả các ảnh của tất cả client
    QString m_winner;
    bool m_voteFinished;
};

#endif // GAMEMANAGER_H
