#ifndef WEBSOCKETHANDLER_H
#define WEBSOCKETHANDLER_H

#include <QObject>
#include <QtWebSockets/QWebSocketServer>
#include <QtWebSockets/QWebSocket>
#include <QMap>
class WebSocketHandler : public QObject
{
    Q_OBJECT
public:
    explicit WebSocketHandler(QObject *parent = nullptr);
    ~WebSocketHandler();

signals:
    // pháy ra tín hiệu này đến MessageProcessorHanler để phân tích tin nhắn
    void newMessageToProcess(QString message);

public slots:
    void onNewSocketConnection();
    void onSocketDisconnected();
    void onTextMessageReceived(QString); // Hàm xử lý Message gửi tới
    //
    void sendTextMessageToClient(QString message , QString clientId); // Gửi tin nhắn đến 1 clientId
    void sendTextMessageToMultiClients(QString message , QStringList clientIDs); // Gửi tin nhán đến tất cả client



private:
    QWebSocketServer* m_socketSever;
    // QMap <client Id , QWebSocket>
    QMap<QString,QWebSocket*> m_clientList;

};

#endif // WEBSOCKETHANDLER_H
