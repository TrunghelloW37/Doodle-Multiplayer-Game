#include "WebSocketHandler.h"

WebSocketHandler::WebSocketHandler(QObject *parent)
    : QObject{parent}
{
    m_webSocket = new QWebSocket;
    connect(m_webSocket, &QWebSocket::connected,this, &WebSocketHandler::onConnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived,this, &WebSocketHandler::onTextMessageReceived);

}

WebSocketHandler::~WebSocketHandler()
{
    m_webSocket->deleteLater();
}

void WebSocketHandler::connectToServer(QString hostAddress)
{
    qDebug() << "QML Call ConnectToServer: " << hostAddress;
    m_webSocket->open(hostAddress);
    // QString serverAddress = "ws://myserver.com";
}

// Ứng dụng khách: Đã thiết lập kết nối
void WebSocketHandler::onConnected()
{
    qDebug() << "Client App: Connection established!";
}

void WebSocketHandler::onTextMessageReceived(QString message)
{
    emit newMessageReadyForProcessing(message); // --> Connect ở main.cpp
}

void WebSocketHandler::sendMessageToServer(QString message)
{
    m_webSocket->sendTextMessage(message);
}


