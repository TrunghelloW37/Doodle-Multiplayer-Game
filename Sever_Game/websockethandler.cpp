#include "websockethandler.h"
#include <random>
#include <QRandomGenerator>
#include <QDateTime>

WebSocketHandler::WebSocketHandler(QObject *parent)
    : QObject{parent}
{
    // “DoodleDrawerServer” là tên của máy chủ Server
    m_socketSever = new QWebSocketServer("DoodleDrawerServer",QWebSocketServer::NonSecureMode, this);
    connect(m_socketSever,&QWebSocketServer::newConnection,this,&WebSocketHandler::onNewSocketConnection);
    // Nhận tật cả Ip trên cổng 8585
    if(m_socketSever->listen(QHostAddress::Any,8585))
    {
        qDebug() << "Server is running";
    }
    else
    {
        qDebug() << "Server unable to start listening for connection!!";
    }
}

WebSocketHandler::~WebSocketHandler()
{
    m_socketSever->deleteLater();
}

void WebSocketHandler::onNewSocketConnection()
{
    qDebug() << "new Client connected!";
    // Tạo số ngẫu nhiên từ (1000 - 9999) để gắn Id cho Client sau mỗi lần chương trình chạy
    std::default_random_engine generator;
    generator.seed(QDateTime::currentMSecsSinceEpoch());
    std::uniform_int_distribution<int> idGenerator(1000,9999);
    QString newClientId = QString::number(idGenerator(generator));

    while(m_clientList.keys().contains(newClientId))
    {
        newClientId = QString::number(idGenerator(generator));
    }

    auto newClient = m_socketSever->nextPendingConnection();
    newClient->setParent(this); // giống setProperty

    // Khi nextClient nhận được một tin nhắn văn bản, nó sẽ phát ra tín hiệu textMessageReceived
    connect(newClient, &QWebSocket::textMessageReceived,this, &WebSocketHandler::onTextMessageReceived);
    // Khi máy chủ nhận được thông báo ngắt kết nối từ máy khách, đối tượng QWebSocket tương ứng trên máy chủ cũng sẽ phát ra tín hiệu disconnected
    connect(newClient, &QWebSocket::disconnected,this,&WebSocketHandler::onSocketDisconnected);
    // Gửi Id sang bên Client để định danh Client(Id)
    newClient->sendTextMessage("type:uniqueId;payLoad:" + newClientId);
    // Thêm client vừa kết nối vào QMap<QString , QWebsocket*>
    m_clientList[newClientId] = newClient;
    qDebug() << "Socket1: Server newClientId: " << newClientId;
}

void WebSocketHandler::onTextMessageReceived(QString message)
{
    emit newMessageToProcess(message); // Tin nhắn mới cần được xử lý
}

// Gửi tin nhắn đến 1 client
void WebSocketHandler::sendTextMessageToClient(QString message, QString clientId)
{
    if(m_clientList.contains(clientId))
    {
        // Gửi tín nhắn đến clientId chỉ định
        QWebSocket* existingClient = m_clientList[clientId];
        existingClient->sendTextMessage(message);
    }
}

// Gửi tin nhắn đến nhiều clients
void WebSocketHandler::sendTextMessageToMultiClients(QString message, QStringList clientIDs)
{
    for(const QString& client : clientIDs)
    {
        sendTextMessageToClient(message , client);
    }
}

// Khi client Distonnection
void WebSocketHandler::onSocketDisconnected()
{
    qDebug() << "Socket4. Client Disconected";
    //sender():lớp QObject. Nó trả về đối tượng (object) nào đã phát ra tín hiệu (signal) hiện tại
    auto client = qobject_cast<QWebSocket*>(sender());
    if(client)
    {
        for(QString key : m_clientList.keys())
        {
            if(m_clientList[key] == client)
            {
                m_clientList.remove(key);
                client->deleteLater();
                break;
            }
        }
    }
}






