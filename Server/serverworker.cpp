#include "serverworker.h"
#include <QDataStream>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
ServerWorker::ServerWorker(QObject *parent)
    : QObject(parent)
    , m_serverSocket(new QTcpSocket(this))
{
    connect(m_serverSocket, &QTcpSocket::readyRead, this, &ServerWorker::receiveJson);
    connect(m_serverSocket, &QTcpSocket::disconnected, this, &ServerWorker::disconnectedFromClient);
    connect(m_serverSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &ServerWorker::error);
    // setup creature and map data by reading their json files
    QString creatureTemp, mapTemp;
    QFile creatureFile, mapFile;
    creatureFile.setFileName(":/resources/json/creature.json");
    mapFile.setFileName(":/resources/json/map.json");
    creatureFile.open(QIODevice::ReadOnly | QIODevice::Text);
    creatureTemp = creatureFile.readAll();
    mapFile.open(QIODevice::ReadOnly | QIODevice::Text);
    creatureFile.close();
    mapTemp = mapFile.readAll();
    mapFile.close();
    creatureJDoc = QJsonDocument::fromJson(creatureTemp.toUtf8());
    mapJDoc = QJsonDocument::fromJson(mapTemp.toUtf8());
    creatureJObj = creatureJDoc.object();
    mapJObj = mapJDoc.object();
}

void ServerWorker::initPlayer(const QJsonObject &json) {
    player = new Player(json);
}

void ServerWorker::sendPlayerStat(const QJsonObject &json) {

}

// need to work on traveling timer
void ServerWorker::movePlayerLoc(const QJsonObject &json) {
    // always use loc_map = alpha-1 for now
    QString loc_map = "alpha-1";
    int loc_x = json.value("loc_x").toInt();
    int loc_y = json.value("loc_y").toInt();
    player->set_player_loc(loc_map, loc_x, loc_y);
}

void ServerWorker::battle(const QJsonObject &json) {
    // always use loc_map = alpha-1 for now
    QString loc_map = "alpha-1";
    int loc_x = json.value("loc_x").toInt();
    int loc_y = json.value("loc_y").toInt();
    // stage coord in QString
    QString stage = QString::number(loc_x) + "," + QString::number(loc_y);
    // get info for what mobs are in this stage
    QJsonArray mobsVal = mapJObj.value(loc_map).toObject().value("stage").toObject().value(stage).toObject().value("mobs").toArray();
    // store mobs name in a QStringList
    QStringList mob_names;
    for (auto it = mobsVal.begin(); it != mobsVal.end(); ++it) {
        mob_names.append(it->toString());
    }
    // create mob objects
    QVector<Creature> mobs;
    for (auto it = mob_names.begin(); it != mob_names.end(); ++it) {
        QJsonObject mob_status = creatureJObj.value(*it).toObject();
        Creature new_mob(mob_status, 1);
        mobs.append(new_mob);
    }
    // setup a battle
    Battle new_battle(player, mobs);
    // now battle
    while(new_battle.is_battle()) {
        new_battle.update();
    }
    // report battle to server
    QString battle_report = new_battle.summarize();
    QJsonObject reportObj;
    reportObj["type"] = "report";
    reportObj["battle"] = battle_report;
//    reportObj["drop"] = drop;
    emit jsonReceived(reportObj);
}

bool ServerWorker::setSocketDescriptor(qintptr socketDescriptor)
{
    return m_serverSocket->setSocketDescriptor(socketDescriptor);
}

void ServerWorker::sendJson(const QJsonObject &json)
{
    QJsonObject status = json.value(QLatin1String("status")).toObject();
    const QByteArray jsonData = QJsonDocument(json).toJson();
    emit logMessage("Sending to " + userName() + " - " + QString::fromUtf8(jsonData));
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    socketStream << jsonData;
}

void ServerWorker::disconnectFromClient()
{
    m_serverSocket->disconnectFromHost();
}

QString ServerWorker::userName() const
{
    m_userNameLock.lockForRead();
    const QString result = m_userName;
    m_userNameLock.unlock();
    return result;
}

void ServerWorker::setUserName(const QString &userName)
{
    m_userNameLock.lockForWrite();
    m_userName = userName;
    m_userNameLock.unlock();
}

void ServerWorker::receiveJson()
{
    QByteArray jsonData;
    QDataStream socketStream(m_serverSocket);
    socketStream.setVersion(QDataStream::Qt_5_7);
    for (;;) {
        socketStream.startTransaction();
        socketStream >> jsonData;
        if (socketStream.commitTransaction()) {
            QJsonParseError parseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &parseError);
            if (parseError.error == QJsonParseError::NoError) {
                if (jsonDoc.isObject())
                    emit jsonReceived(jsonDoc.object());
                else
                    emit logMessage("Invalid message: " + QString::fromUtf8(jsonData));
            } else {
                emit logMessage("Invalid message: " + QString::fromUtf8(jsonData));
            }
        } else {
            break;
        }
    }
}


