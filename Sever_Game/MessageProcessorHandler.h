#ifndef MESSAGEPROCESSORHANDLER_H
#define MESSAGEPROCESSORHANDLER_H

#include <QObject>

// MessageProcessorHandler : Xử lý tin nhắn
class MessageProcessorHandler : public QObject
{
    Q_OBJECT
public:
    explicit MessageProcessorHandler(QObject *parent = nullptr);

public slots:
    void processMessage(QString message); // Hàm xử lý tin nhắn

signals:
    void createGameRequest(QString uniqueId); // UniqueId : Id duy nhất
    void joinGameLobbyRequest(QString lobbyId ,QString uniqueId); // lobby = sảnh chờ
    void messageLobbyToRequest(QString message , QString lobbyId , QString senderId); // tin nhắn - Id phòng - Id người gửi
    void clientReadyToPlay(QString checkReady , QString uniqueId); //
    void newImageData(QString dataImage , QString clientId); // ẢNh lúc bắt đầu ( nhấn nút save)
    void newDrawData(QString dataImage , QString clientId); //  ảnh vẽ đề tài hoàn thiện(nhấn nút Done)
    void newVote(QString imageVote , QString clientId);
#endif // MESSAGEPROCESSORHANDLER_H
};
