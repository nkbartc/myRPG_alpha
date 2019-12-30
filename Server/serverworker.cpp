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
}

void ServerWorker::initPlayer(const QJsonObject &json) {
    player = new Player(json);
}

void ServerWorker::sendPlayerStat(const QJsonObject &json) {

}

// need to work on traveling timer
void ServerWorker::movePlayerLoc(const QJsonObject &json) {
    QString loc_map = "alpha-1";
    int loc_x = json.value("loc_x").toInt();
    int loc_y = json.value("loc_y").toInt();
    player->set_player_loc(loc_map, loc_x, loc_y);
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


