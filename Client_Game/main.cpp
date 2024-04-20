#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "Controllers/WebSocketHandler.h"
#include "Controllers/GameManager.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    WebSocketHandler socketHandler;
    socketHandler.connectToServer("ws://127.0.0.1:8585");

    GameManager gameManager;
    QObject::connect(&socketHandler,&WebSocketHandler::newMessageReadyForProcessing,
                     &gameManager,&GameManager::processSocketMessage);

    QObject::connect(&gameManager,&GameManager::newMessageReadyToSend,
                     &socketHandler,WebSocketHandler::sendMessageToServer);


    QQmlContext* context = engine.rootContext();
    context->setContextProperty("webSocketHandler",&socketHandler);
    context->setContextProperty("gameManager",&gameManager);

    //engine.rootContext()->setContextProperty("gameManager",&gameManager);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);


    return app.exec();
}
