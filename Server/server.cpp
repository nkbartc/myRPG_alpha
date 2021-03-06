#include "server.h"
#include "serverworker.h"
#include <QThread>
#include <functional>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QTimer>

Server::Server(QObject *parent)
    : QTcpServer(parent)
    , m_idealThreadCount(qMax(QThread::idealThreadCount(), 1))
{
    m_availableThreads.reserve(m_idealThreadCount);
    m_threadsLoad.reserve(m_idealThreadCount);

    // open database
    QSqlDatabase mydb = QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName("C:/Users/nkbar/Documents/SQLite/myRPG.db");
    if(!mydb.open()) {
        qDebug() << "DB status: Failed to open the DB";
    } else {
        qDebug() << "DB status: Connected...";
    }
}

Server::~Server()
{
    for (QThread *singleThread : m_availableThreads) {
        singleThread->quit();
        singleThread->wait();
    }
}
void Server::incomingConnection(qintptr socketDescriptor)
{
    ServerWorker *worker = new ServerWorker;
    if (!worker->setSocketDescriptor(socketDescriptor)) {
        worker->deleteLater();
        return;
    }
    int threadIdx = m_availableThreads.size();
    if (threadIdx < m_idealThreadCount) { //we can add a new thread
        m_availableThreads.append(new QThread(this));
        m_threadsLoad.append(1);
        m_availableThreads.last()->start();
    } else {
        // find the thread with the least amount of clients and use it
        threadIdx = std::distance(m_threadsLoad.cbegin(), std::min_element(m_threadsLoad.cbegin(), m_threadsLoad.cend()));
        ++m_threadsLoad[threadIdx];
    }
    worker->moveToThread(m_availableThreads.at(threadIdx));
    connect(m_availableThreads.at(threadIdx), &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &ServerWorker::disconnectedFromClient, this, std::bind(&Server::userDisconnected, this, worker, threadIdx));
    connect(worker, &ServerWorker::error, this, std::bind(&Server::userError, this, worker));
    connect(worker, &ServerWorker::jsonReceived, this, std::bind(&Server::jsonReceived, this, worker, std::placeholders::_1));
    connect(this, &Server::stopAllClients, worker, &ServerWorker::disconnectFromClient);
    m_clients.append(worker);
    emit logMessage("New client Connected");
}
void Server::sendJson(ServerWorker *destination, const QJsonObject &message)
{
    Q_ASSERT(destination);
    QTimer::singleShot(0, destination, std::bind(&ServerWorker::sendJson, destination, message));
}
void Server::broadcast(const QJsonObject &message, ServerWorker *exclude)
{
    for (ServerWorker *worker : m_clients) {
        Q_ASSERT(worker);
        if (worker == exclude)
            continue;
        sendJson(worker, message);
    }
}

void Server::jsonReceived(ServerWorker *sender, const QJsonObject &json)
{
    Q_ASSERT(sender);
    emit logMessage("JSON received " + QString::fromUtf8(QJsonDocument(json).toJson()));
    if (sender->userName().isEmpty())
        return jsonFromLoggedOut(sender, json);
    jsonFromLoggedIn(sender, json);
}

void Server::userDisconnected(ServerWorker *sender, int threadIdx)
{
    --m_threadsLoad[threadIdx];
    m_clients.removeAll(sender);
    const QString userName = sender->userName();
    if (!userName.isEmpty()) {
        QJsonObject disconnectedMessage;
        disconnectedMessage["type"] = QStringLiteral("userdisconnected");
        disconnectedMessage["username"] = userName;
        broadcast(disconnectedMessage, nullptr);
        emit logMessage(userName + " disconnected");
    }
    sender->deleteLater();
}

void Server::userError(ServerWorker *sender)
{
    Q_UNUSED(sender)
    emit logMessage("Error from " + sender->userName());
}

void Server::stopServer()
{
    emit stopAllClients();
    close();
}

void Server::getCharacter(ServerWorker *sender, const int userId) {
    QSqlQuery qry;
//    qry.exec("select CharacterId from Character where UserId='"+ QString::number(userId) + "'");
//    qry.next();
//    // assume always the first character for testing.
//    QString charId = qry.value(0).toString();
    qry.exec("select * from Character where UserId='"+ QString::number(userId) + "'");
    qry.next();
    QJsonObject message;
    QJsonObject status;
    // massge = {"type":"charInfo",
    //           "status":{status}
    //          }
    qDebug() << "getCharacter start";
    message["type"] = QStringLiteral("charInfo");
    status["userId"] = qry.value("UserId").toInt();
    status["characterId"] = qry.value("CharacterId").toInt();
    qDebug() << "characterId: " + QString::number(qry.value("CharacterId").toInt());
    status["class"] = qry.value("Class").toString();
    qDebug() << "class: " + qry.value("Class").toString();
    status["name"] = qry.value("Name").toString();
    //  status_.time
    status["lv"] = qry.value("Lv").toInt();
    status["exp"] = qry.value("Exp").toInt();
    status["reqExp"] = qry.value("Req_Exp").toInt();
    status["fame"] = qry.value("Fame").toInt();
    status["gold"] = qry.value("Gold").toInt();
    status["diamond"] = qry.value("Diamond").toInt();
    status["hp"] = qry.value("Hp").toInt();
    status["atk"] = qry.value("Atk").toInt();
    status["def"] = qry.value("Def").toInt();
    status["mdef"] = qry.value("MDef").toInt();
    status["speed"] = qry.value("Speed").toInt();
    status["dodge"] = qry.value("Dodge").toInt();
    status["ac"] = qry.value("Ac").toInt();
    status["crit"] = qry.value("Crit").toInt();
    status["block"] = qry.value("Block").toInt();
    status["loc_map"] = qry.value("Loc_map").toString();
    status["loc_x"] = qry.value("Loc_x").toInt();
    status["loc_y"] = qry.value("Loc_y").toInt();
    message["status"] = status;
    qDebug() << "getCharacter end";
    // status_gears implement them all later

    sender->initPlayer(message);
    sendJson(sender, message);
}

bool Server::newCharacter(QString newUserName) {
    QSqlQuery qry;
    if(qry.exec("select UserId from UserInfo where UserName='"+ newUserName + "'")) {
        int count = 0;
        while (qry.next()) {
            count++;
        }
        if (count < 1) {
            qDebug() << "Cannot find your account";
            QJsonObject message;
            message["type"] = QStringLiteral("login");
            message["success"] = false;
            message["reason"] = QStringLiteral("Username or Password is incorrect");
            return false;
        }
    }
    qry.first();
    int userId = qry.value(0).toInt();
    qry.prepare("INSERT INTO Character ("
                "UserId,"
                "CharacterId,"
                "Class,"
                "Name,"
                "Time,"
                "Lv,"
                "Exp,"
                "Req_Exp,"
                "Fame,"
                "Gold,"
                "Diamond,"
                "Hp,"
                "Atk,"
                "Def,"
                "MDef,"
                "Speed,"
                "Dodge,"
                "Ac,"
                "Crit,"
                "Block,"
                "Loc_map,"
                "Loc_x,"
                "Loc_y)"
                "VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?);");
    // all new characters are warriors for now
    qry.addBindValue(userId); // UserId
    // all user have only 1 character for now
    qry.addBindValue(1); // CharacterId
    qry.addBindValue("Warrior"); // Class
    qry.addBindValue(newUserName); // Name
    qry.addBindValue(0); // Time
    qry.addBindValue(1); // Lv
    qry.addBindValue(0); // Exp
    qry.addBindValue(2); // Req_Exp
    qry.addBindValue(0); // Fame
    qry.addBindValue(0); // Gold
    qry.addBindValue(0); // Diamond
    qry.addBindValue(30); // Hp
    qry.addBindValue(5); // Atk
    qry.addBindValue(0); // Def
    qry.addBindValue(0); // MDef
    qry.addBindValue(1); // Speed
    qry.addBindValue(0); // Dodge
    qry.addBindValue(0); // Ac
    qry.addBindValue(0); // Crit
    qry.addBindValue(0); // Block
    qry.addBindValue("alpha-1"); // Loc_map
    qry.addBindValue(15); // Loc_x
    qry.addBindValue(15); // Loc_y
    if(!qry.exec()) {
        return false;
    }
    return true;
}

bool Server::checkUserInfo_signup(ServerWorker *sender, const QString newUserName, const QString newPassWord) {
    QSqlQuery qry;
    if(qry.exec("select UserId from UserInfo where UserName='"+ newUserName + "'")) {
        int count = 0;
        while (qry.next()) {
            count++;
        }
        if (count > 0) {
            qDebug() << "Username already exists";
            QJsonObject message;
            message["type"] = QStringLiteral("signup");
            message["success"] = false;
            message["reason"] = QStringLiteral("Username already exists");
            sendJson(sender, message);
            return false;
        }
    }
    if (newUserName.isEmpty()) {
        return false;
    }

    qry.prepare("INSERT INTO UserInfo ("
                "UserName,"
                "Password)"
                "VALUES (?,?);");
    qry.addBindValue(newUserName);
    qry.addBindValue(newPassWord);
    if(!qry.exec()) {
        QJsonObject message;
        message["type"] = QStringLiteral("signup");
        message["success"] = false;
        message["reason"] = QStringLiteral("Failed to sign up");
        sendJson(sender, message);
        return false;
    }
    if (!newCharacter(newUserName)) {
        QJsonObject message;
        message["type"] = QStringLiteral("signup");
        message["success"] = false;
        message["reason"] = QStringLiteral("Failed to new character");
        sendJson(sender, message);
        return false;
    }
    return true;
}

int Server::checkUserInfo_login(ServerWorker *sender, const QString newUserName, const QString newPassWord) {
    QSqlQuery qry;
    if(qry.exec("select UserId from UserInfo where UserName='"+ newUserName +
                "' and password='"+ newPassWord + "'")) {
        int count = 0;
        while (qry.next()) {
            count++;
        }
        if (count < 1) {
            qDebug() << "Username or Password is incorrect";
            QJsonObject message;
            message["type"] = QStringLiteral("login");
            message["success"] = false;
            message["reason"] = QStringLiteral("Username or Password is incorrect");
            sendJson(sender, message);
            return -1;
        }
    }
    if (newUserName.isEmpty())
        return -1;
    QJsonArray allUserNames;
    for (ServerWorker *worker : qAsConst(m_clients)) {
        allUserNames.push_back(worker->userName());
        if (worker == sender)
            continue;
        if (worker->userName().compare(newUserName, Qt::CaseInsensitive) == 0) {
            qDebug() << "User already logged in";
            QJsonObject message;
            message["type"] = QStringLiteral("login");
            message["success"] = false;
            message["reason"] = QStringLiteral("User already logged in");
            sendJson(sender, message);
            return -1;
        }
    }
    qDebug() << "User successfully log in";
    sender->setUserName(newUserName);
    allUserNames.push_front(newUserName);
    QJsonObject successMessage;
    successMessage["type"] = QStringLiteral("login");
    successMessage["success"] = true;
    successMessage["allUserNames"] = allUserNames;
    sendJson(sender, successMessage);
    QJsonObject connectedMessage;
    connectedMessage["type"] = QStringLiteral("newuser");
    connectedMessage["username"] = newUserName;
    connectedMessage["allUserNames"] = allUserNames;
    broadcast(connectedMessage, sender);

    // not sure if this works correctly
    qry.first();
    int userId = qry.value(0).toInt();
//    qDebug() << "userId: " + QString::number(userId);
//    qDebug() << qry.value(1).toString();
//    qDebug() << qry.value(2).toString();
    return userId;
}

void Server::jsonFromLoggedOut(ServerWorker *sender, const QJsonObject &docObj)
{
    Q_ASSERT(sender);
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString())
        return;
    if (typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) != 0 &&
        typeVal.toString().compare(QLatin1String("signup"), Qt::CaseInsensitive) != 0)
        return;
    const QJsonValue usernameVal = docObj.value(QLatin1String("username"));
    const QJsonValue passwordVal = docObj.value(QLatin1String("password"));
    if (usernameVal.isNull() || !usernameVal.isString())
        return;

    const QString newUserName = usernameVal.toString().simplified();
    const QString newPassWord = passwordVal.toString().simplified();

    if (typeVal.toString().compare(QLatin1String("login"), Qt::CaseInsensitive) == 0) {
        int userId = checkUserInfo_login(sender, newUserName, newPassWord);
        if (userId < 0) {
            return;
        }
        getCharacter(sender, userId);
    } else if (typeVal.toString().compare(QLatin1String("signup"), Qt::CaseInsensitive) == 0) {
        bool result = checkUserInfo_signup(sender, newUserName, newPassWord);
        if (!result) {
            return;
        }
        QJsonObject message;
        message["type"] = QStringLiteral("signup");
        message["success"] = true;
        sendJson(sender, message);
    }

}

void Server::jsonFromLoggedIn(ServerWorker *sender, const QJsonObject &docObj)
{
    Q_ASSERT(sender);
    const QJsonValue typeVal = docObj.value(QLatin1String("type"));
    if (typeVal.isNull() || !typeVal.isString()) {
        return;
    }
    if (typeVal.toString() == "message") {
        const QJsonValue textVal = docObj.value(QLatin1String("text"));
        if (textVal.isNull() || !textVal.isString())
            return;
        const QString text = textVal.toString().trimmed();
        if (text.isEmpty())
            return;
        QJsonObject message;
        message["type"] = QStringLiteral("message");
        message["text"] = text;
        message["sender"] = sender->userName();
        broadcast(message, sender);
    } else if (typeVal.toString() == "command") {
        const QJsonValue actionVal = docObj.value("action");
        if (actionVal.toString() == "move") {
            sender->movePlayerLoc(docObj);
            sendJson(sender, docObj);
        } else if (actionVal.toString() == "fight") {
            qDebug() << "server call worker to start a battle";
            sender->battle(docObj);
        }
    } else if (typeVal.toString() == "report") {
        qDebug() << "server call sendJson to send report";
        sendJson(sender, docObj);
    }
}


