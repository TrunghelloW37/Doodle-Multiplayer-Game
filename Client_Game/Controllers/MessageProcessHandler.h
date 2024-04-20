#ifndef MESSAGEPROCESSHANDLER_H
#define MESSAGEPROCESSHANDLER_H

#include <QObject>

class MessageProcessHandler : public QObject
{
    Q_OBJECT
public:
    explicit MessageProcessHandler(QObject *parent = nullptr);

public slots:
    void processMessage(QString message);

signals:
    void uniqueIdRegistration(QString uniqueId);
    void newLobby(QString lobbyId , QStringList clients); // Tín hiệu phòng RoomId
    void lobbyListUpdated(QStringList clients); // Tín hiệu để cập nhật Id client tham gia vào phòng
    void newLobbyMessage(QString message); // Tín hiệu để cập nhật đoạn chat giữa các client
    void readyListChanged(QStringList readyList); // Tín hiệu để cập nhật các client ai đã ready
    void gameStarted(); // Phát ra tín hiệu khi tất cả client đã Sẵn sàng
    // Tín hiệu để chuyển dữ liệu hình ảnh và chủ đề cho Client
    void dataImageAndInstructuon(QString payLoad, QString instruction);
    // Tín hiệu để chuyển  tất cả dữ liệu hình ảnh và tất cả  ClientId cho từng client
    void gameDrawAllReady(QStringList images, QStringList clients);
    void winnerChosen(QString winner);
};

#endif // MESSAGEPROCESSHANDLER_H
