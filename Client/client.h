#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QLineEdit>
#include <QTcpSocket>
#include <QHostAddress>
class QHostAddress;
class QJsonDocument;
class Client : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Client)
public:
    explicit Client(QObject *parent = nullptr);
public slots:
    void connectToServer(const QHostAddress &address, quint16 port);
    void login(const QList<QLineEdit *> &fields);
    void sendCommand(const QJsonObject &command);
    void sendMessage(const QString &text);
    void disconnectFromHost();
private slots:
    void onReadyRead();
signals:
    void connected();
    void loggedIn();
    void loginError(const QString &reason);
    void disconnected();
    void messageReceived(const QString &sender, const QString &text);
    void error(QAbstractSocket::SocketError socketError);
    void userJoined(const QString &username, const QStringList &allUserNames);
    void userLeft(const QString &username);
    void getPlayerStat(const QJsonObject &docObj);
    void getPlayerLoc(const QString loc_x, const QString loc_y);
private:
    QTcpSocket *m_clientSocket;
    bool m_loggedIn;
    void jsonReceived(const QJsonObject &doc);
};

#endif // CLIENT_H
