#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
#include <QVector>
#include <QSqlDatabase>
#include <QtSql>
#include <QtDebug>
#include <QFileInfo>
#include <iostream>
class QThread;
class ServerWorker;
class QJsonObject;
class Server : public QTcpServer
{
    Q_OBJECT
    Q_DISABLE_COPY(Server)
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
protected:
    void incomingConnection(qintptr socketDescriptor) override;
private:
    const int m_idealThreadCount;
    QVector<QThread *> m_availableThreads;
    QVector<int> m_threadsLoad;
    QVector<ServerWorker *> m_clients;
private slots:
    void broadcast(const QJsonObject &message, ServerWorker *exclude);
    void jsonReceived(ServerWorker *sender, const QJsonObject &doc);
    void userDisconnected(ServerWorker *sender, int threadIdx);
    void userError(ServerWorker *sender);
public slots:
    void stopServer();
private:
    void jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &doc);
    void jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &doc);
    void sendJson(ServerWorker *destination, const QJsonObject &message);
    int checkUserInfo_login(ServerWorker *sender, const QString newUserName, const QString newPassWord);
    bool checkUserInfo_signup(ServerWorker *sender, const QString newUserName, const QString newPassWord);
    void getCharacter(ServerWorker *sender, const int userId);
    bool newCharacter(QString newUserName);

signals:
    void logMessage(const QString &msg);
    void stopAllClients();
};

#endif // CHATSERVER_H
