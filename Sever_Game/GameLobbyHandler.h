#ifndef GAMELOBBYHANDLER_H
#define GAMELOBBYHANDLER_H

#include <QObject>
#include <QMap>
class GameLobbyHandler : public QObject
{
    Q_OBJECT
public:
    // lobbyId là Id phòng
    explicit GameLobbyHandler(QString lobbyId , QObject *parent );
    void addClient(QString clientId);
    QString clientsInLobby();
    QStringList clientsInLobbyList();
    void userReadyToPlay(QString checkReady,QString clientId);
    QString whoIsReady();
    void newImageData(QString imageData , QString clientId); // ảnh mới bắt đầu vẽ
    void newDrawData(QString imageData , QString clientId); // ảnh đã vẽ xong ( vẽ theo đề tài)
    void choosenObject(); // Lựa chọn ngẫu nhiền đề tài vẽ
    QString drawInstruction(); // Trả về chủ đề tài để vẽ
    void newVote(QString voteImage , QString clientId);

signals:
    void userReadyListChanged();
    void gameReadyToBegin();
    void allDataImageReceived(QMap<QString,QString> newMap); // Tín hiệu khi các client đều có dữ liệu ảnh ( ảnh chưa hoàn thiện)
    void allDataDrawReceived(QMap<QString,QString> newMap); // Tín hiệu khi các client đều có dữ liệu ảnh ( ảnh đã vẽ xong)
    void winnerChosen(QString winner);
private:
    QString m_LobbyId; // Id phòng
    QList<QString> m_gameClientList; // chứa các người chơi trong phòng
    QMap<QString,bool> m_clientReadyList; // các người chơi trong phòng đã sẵn sàng
    QMap<QString,QString> m_clientDataImage; // clientId -- dataImage ( ảnh chưa vẽ xong)
    QMap<QString,QString> m_clientDrawImage; // clientId -- dataImage ( ảnh đã vẽ xong theo đề tài)
    QStringList m_objectsToDraw;  // Chứa các chủ đề để vẽ hình
    QString m_selectedObject; // lưu trữ đề tài
    QList<QString>m_clientsWhoVoted; // QList dùng để chứa các client đã vote
    QMap<QString,int> m_voteResult;

};

#endif // GAMELOBBYHANDLER_H
