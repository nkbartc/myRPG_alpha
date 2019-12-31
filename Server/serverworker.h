#ifndef SERVERWORKER_H
#define SERVERWORKER_H

#include <QObject>
#include <QTcpSocket>
#include <QReadWriteLock>

#include "player.h"
#include "creature.h"
#include "battle.h"
class Player;
class Creature;
class Battle;
class QJsonObject;
class ServerWorker : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(ServerWorker)
public:
    explicit ServerWorker(QObject *parent = nullptr);
    virtual bool setSocketDescriptor(qintptr socketDescriptor);
    QString userName() const;
    void setUserName(const QString &userName);
    void sendJson(const QJsonObject &json);
    void initPlayer(const QJsonObject &json);
    void sendPlayerStat(const QJsonObject &json);
    void movePlayerLoc(const QJsonObject &json);
    void battle(const QJsonObject &json);
public slots:
    void disconnectFromClient();
private slots:
    void receiveJson();
signals:
    void jsonReceived(const QJsonObject &jsonDoc);
    void disconnectedFromClient();
    void error();
    void logMessage(const QString &msg);
private:
    QTcpSocket *m_serverSocket;
    QString m_userName;
    mutable QReadWriteLock m_userNameLock;
    Player *player;
    QJsonDocument creatureJDoc, mapJDoc;
    QJsonObject creatureJObj, mapJObj;
};

#endif // SERVERWORKER_H
