#include "clientwindow.h"
#include "ui_clientwindow.h"


ClientWindow::ClientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ClientWindow)
    , m_client(new Client(this))
    , m_chatModel(new QStandardItemModel(this))
    , m_playerModel(new QStandardItemModel(this))
{
    // set up of the .ui file
    ui->setupUi(this);
    // set up toolbar
    setupToolBar();
    // set up background
//    setupBackground();
    // the model for the messages and players will have 1 column
    m_chatModel->insertColumn(0);
    m_playerModel->insertColumn(0);
    // set the models as the data source for the list view
    ui->listView_chat->setModel(m_chatModel);
    ui->listView_player->setModel(m_playerModel);
    // connect the signals from the chat client to the slots in this ui
    connect(m_client, &Client::connected, this, &ClientWindow::connectedToServer);
    connect(m_client, &Client::loggedIn, this, &ClientWindow::loggedIn);
    connect(m_client, &Client::loginError, this, &ClientWindow::loginFailed);
    connect(m_client, &Client::signedUp, this, &ClientWindow::signedUp);
    connect(m_client, &Client::signUpError, this, &ClientWindow::signUpFailed);
    connect(m_client, &Client::messageReceived, this, &ClientWindow::messageReceived);
    connect(m_client, &Client::disconnected, this, &ClientWindow::disconnectedFromServer);
    connect(m_client, &Client::error, this, &ClientWindow::error);
    connect(m_client, &Client::userJoined, this, &ClientWindow::userJoined);
    connect(m_client, &Client::userLeft, this, &ClientWindow::userLeft);
    connect(m_client, &Client::getPlayerStat, this, &ClientWindow::getPlayerStat);
    connect(m_client, &Client::getPlayerLoc, this, &ClientWindow::getPlayerLoc);
    connect(m_client, &Client::getBattleReport, this, &ClientWindow::getBattleReport);
    // connect the press of the enter while typing to the slot that sends the message
    connect(ui->lineEdit_chat, &QLineEdit::returnPressed, this, &ClientWindow::sendMessage);
}

ClientWindow::~ClientWindow()
{
    delete ui;
}

void ClientWindow::setupBackground() {
    QPixmap background(":/resources/toolbar/landscape_rand1.png");
//    background = background.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
}

void ClientWindow::setupToolBar() {
    QToolBar *accountToolBar = addToolBar(tr("Accout"));

    const QIcon signupIcon = QIcon::fromTheme("account-signup", QIcon(":/resources/toolbar/signup.png"));
    QAction *signupAct = new QAction(signupIcon, tr("&signup"), this);
    accountToolBar->addAction(signupAct);

    const QIcon loginIcon = QIcon::fromTheme("account-login", QIcon(":/resources/toolbar/login.png"));
    QAction *loginAct = new QAction(loginIcon, tr("&login"), this);
    connect(loginAct, &QAction::triggered, this, &ClientWindow::attemptConnection);
    accountToolBar->addAction(loginAct);

    const QIcon logoutIcon = QIcon::fromTheme("account-logout", QIcon(":/resources/toolbar/logout.png"));
    QAction *logoutAct = new QAction(logoutIcon, tr("&logout"), this);
    connect(logoutAct, &QAction::triggered, m_client, &Client::disconnectFromHost);
    accountToolBar->addAction(logoutAct);
}


// update_XX_client for GUI, update_XX_db for DB
// todo: update_XX_db
void ClientWindow::update() {

}

void ClientWindow::setupMap(const QString map) {
    // :/resources/SampleMap/alpha-1.png
    qDebug() << map;
    srcImage = QPixmap(":/resources/SampleMap/" + map + ".png");
    qDebug() << "map width: " + QString::number(srcImage.width());
    qDebug() << "map height " + QString::number(srcImage.height());
    scene = new QGraphicsScene(this);
    scene->addPixmap(srcImage);
    scene->installEventFilter(this);
    ui->graphicsView_map->setScene(scene);
    ui->graphicsView_map->setAlignment(Qt::AlignTop|Qt::AlignLeft);
}

void ClientWindow::getPlayerStat(const QJsonObject &json) {
    qDebug() << "ClientWindow::getPlayerStat";
    ui->label_userId->setNum(json.value("userId").toInt());
    ui->label_class->setText(json.value("class").toString());
    ui->label_name->setText(json.value("name").toString());
  //  player_.status_.time
    ui->label_lv->setNum(json.value("lv").toInt());
    ui->label_exp->setNum(json.value("exp").toInt());
    ui->label_reqExp->setNum(json.value("reqExp").toInt());
    ui->label_fame->setNum(json.value("fame").toInt());
    ui->label_gold->setNum(json.value("gold").toInt());
    ui->label_diamond->setNum(json.value("diamond").toInt());
    ui->label_hp->setNum(json.value("hp").toInt());
    ui->label_atk->setNum(json.value("atk").toInt());
    ui->label_def->setNum(json.value("def").toInt());
    ui->label_mdef->setNum(json.value("mdef").toInt());
    ui->label_speed->setNum(json.value("speed").toInt());
    ui->label_dodge->setNum(json.value("dodge").toInt());
    ui->label_ac->setNum(json.value("ac").toInt());
    ui->label_crit->setNum(json.value("crit").toInt());
    ui->label_block->setNum(json.value("block").toInt());
    // character's location
    QString loc_map = json.value("loc_map").toString();
    cur_x = json.value("loc_x").toInt();
    cur_y = json.value("loc_y").toInt();
    QString location = loc_map + ":" + QString::number(cur_x) + "," + QString::number(cur_y);
    setupMap(loc_map);
    ui->label_location->setText(location);
}

void ClientWindow::getPlayerLoc(const QString loc_x, const QString loc_y) {
    QString location = "alpha-1:" + loc_x + "," + loc_y;
    cur_x = loc_x.toInt();
    cur_y = loc_y.toInt();
    ui->label_location->setText(location);
}

void ClientWindow::getBattleReport(const QString battleReport) {
    ui->textBrowser_combat->setText(battleReport);
}

void ClientWindow::update_bag_client() {

}

void ClientWindow::update_combat_log(QString report) {
  ui->textBrowser_combat->setText(report);
  QScrollBar* sb = ui->textBrowser_combat->verticalScrollBar();
  sb->setValue(sb->maximum());
}

void ClientWindow::on_pushButton_clear_combat_clicked()
{
    this->update_combat_log("");
}

void ClientWindow::setup_tiled_map() {
    srcImage = QPixmap(":/resources/SampleMap/alpha-2.png");
    qDebug() << "map width: " + QString::number(srcImage.width());
    qDebug() << "map height " + QString::number(srcImage.height());
    scene = new QGraphicsScene(this);
    scene->addPixmap(srcImage);
    scene->installEventFilter(this);
    ui->graphicsView_map->setScene(scene);
    ui->graphicsView_map->setAlignment(Qt::AlignTop|Qt::AlignLeft);
}

bool ClientWindow::eventFilter(QObject *target, QEvent *event) {
    if (target == scene) {
        if (event->type() == QEvent::GraphicsSceneMousePress) {
            QGraphicsSceneMouseEvent* me = static_cast<QGraphicsSceneMouseEvent*>(event);
            mouse_pos_map = me->scenePos();
            int click_x = int(mouse_pos_map.x() / 48);
            int click_y = int(mouse_pos_map.y() / 48);
            qDebug() << "(" + QString::number(click_x)
                      + "," + QString::number(click_y)
                      + ")";
            QDialog dialog(this);
            QFormLayout form(&dialog);
            QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                       Qt::Horizontal, &dialog);
            QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
            QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
            // pop a dialog to ask client whether to go or not.
            if (click_x == cur_x && click_y == cur_y) {
                form.addRow(new QLabel("Start the fight?"));
                form.addRow(&buttonBox);
                int ok_cancel = dialog.exec();
                if (ok_cancel == QDialog::Accepted) {
                    QJsonObject command;
                    command["type"] = QStringLiteral("command");
                    command["action"] = "fight";
                    command["loc_x"] = click_x;
                    command["loc_y"] = click_y;
                    m_client->sendCommand(command);
                }
            } else {
                QString destination = "(" + QString::number(click_x) + "," + QString::number(click_y) + ")";
                int estimated_time = abs(click_x - cur_x) + abs(click_y - cur_y);
                form.addRow(new QLabel("Travel to: " + destination));
                form.addRow(new QLabel("Estimated time: " + QString::number(estimated_time) + " sec"));
                form.addRow(&buttonBox);

                int ok_cancel = dialog.exec();
                if (ok_cancel == QDialog::Accepted) {
                    QJsonObject command;
                    command["type"] = QStringLiteral("command");
                    command["action"] = "move";
                    command["loc_x"] = click_x;
                    command["loc_y"] = click_y;
                    m_client->sendCommand(command);
                }
            }
        }
    }
    return QMainWindow::eventFilter(target, event);
}


void ClientWindow::attemptConnection()
{
    // We ask the user for the address of the server, we use 127.0.0.1 (aka localhost) as default
    const QString hostAddress = QInputDialog::getText(
        this
        , tr("Chose Server")
        , tr("Server Address")
        , QLineEdit::Normal
        , QStringLiteral("127.0.0.1")
    );
    if (hostAddress.isEmpty())
        return; // the user pressed cancel or typed nothing
    // tell the client to connect to the host using the port 22222
    m_client->connectToServer(QHostAddress(hostAddress), 22222);
}

void ClientWindow::signUp() {
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    // Add the lineEdits with their respective labels
    QList<QLineEdit *> fields;

    QLineEdit *lineEdit1 = new QLineEdit(&dialog);
    QString label1 = QString("Username");
    form.addRow(label1, lineEdit1);
    fields << lineEdit1;

    QLineEdit *lineEdit2 = new QLineEdit(&dialog);
    lineEdit2->setEchoMode(QLineEdit::Password);
    QString label2 = QString("Password");
    form.addRow(label2, lineEdit2);
    fields << lineEdit2;

    QLineEdit *lineEdit3 = new QLineEdit(&dialog);
    lineEdit3->setEchoMode(QLineEdit::Password);
    QString label3 = QString("Re-Password");
    form.addRow(label3, lineEdit3);
    fields << lineEdit3;

    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // execute dialog and store user's decision
    int ok_cancel = dialog.exec();

    // ok -> attemptLogin
    if (ok_cancel == QDialog::Accepted) {
        // chick if password and Re-password are the same
        QString password = fields.at(1)->text();
        QString re_password = fields.at(2)->text();
        if (password != re_password) {
            QMessageBox::warning(this, tr("Error"),tr("Given passwords don't match!"));
            // redirect user back to signup
            signUp();
        }
        // try to register with the given username and password
        attemptSignUp(fields);
    }
}

void ClientWindow::connectedToServer() {
    QDialog dialog(this);
    // Use a layout allowing to have a label next to each field
    QFormLayout form(&dialog);

    // Add the lineEdits with their respective labels
    QList<QLineEdit *> fields;

    QLineEdit *lineEdit1 = new QLineEdit(&dialog);
    QString label1 = QString("Username");
    form.addRow(label1, lineEdit1);
    fields << lineEdit1;

    QLineEdit *lineEdit2 = new QLineEdit(&dialog);
    lineEdit2->setEchoMode(QLineEdit::Password);
    QString label2 = QString("Password");
    form.addRow(label2, lineEdit2);
    fields << lineEdit2;


    // Add some standard buttons (Cancel/Ok) at the bottom of the dialog
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    QPushButton *signUpButton = new QPushButton("Sign Up", this);
    connect(signUpButton, SIGNAL(clicked()), this, SLOT(signUp()));
    form.addRow(&buttonBox, signUpButton);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    // execute dialog and store user's decision
    int ok_cancel = dialog.exec();

    // cancel -> disconnectFromHost
    if (ok_cancel == QDialog::Rejected) {
        m_client->disconnectFromHost();
    } else if (ok_cancel == QDialog::Accepted) {
        // try to login with the given username and password
        attemptLogin(fields);
    }
}
void ClientWindow::attemptSignUp(const QList<QLineEdit *> &fields) {
    m_client->signup(fields);
}

void ClientWindow::attemptLogin(const QList<QLineEdit *> &fields)
{
    // use the client to attempt a log in with the given username
    m_client->login(fields);
}

void ClientWindow::loggedIn()
{
    // clear the user name record
    m_lastUserName.clear();

    ui->lineEdit_chat->setEnabled(true);
    ui->listView_chat->setEnabled(true);
}

void ClientWindow::loginFailed(const QString &reason)
{
    // the server rejected the login attempt
    // display the reason for the rejection in a message box
    QMessageBox::critical(this, tr("Error"), reason);

    m_client->disconnectFromHost();
}

void ClientWindow::signedUp() {
    QMessageBox::warning(this, tr("Success"),tr("You've successfully signed up!"));
}
void ClientWindow::signUpFailed(const QString &reason) {
    QByteArray reason_temp = reason.toLocal8Bit();
    const char *reason_tr = reason_temp.data();
    QMessageBox::warning(this, tr("Error"),tr(reason_tr));
    // redirect user back to signup
    signUp();
}

void ClientWindow::messageReceived(const QString &sender, const QString &text)
{
    // store the index of the new row to append to the model containing the messages
    int newRow = m_chatModel->rowCount();
    // we display a line containing the username only if it's different from the last username we displayed
    if (m_lastUserName != sender) {
        // store the last displayed username
        m_lastUserName = sender;
        // create a bold default font
        QFont boldFont;
        boldFont.setBold(true);
        // insert 2 row, one for the message and one for the username
        m_chatModel->insertRows(newRow, 2);
        // store the username in the model
        m_chatModel->setData(m_chatModel->index(newRow, 0), sender + ':');
        // set the alignment for the username
        m_chatModel->setData(m_chatModel->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
        // set the for the username
        m_chatModel->setData(m_chatModel->index(newRow, 0), boldFont, Qt::FontRole);
        ++newRow;
    } else {
        // insert a row for the message
        m_chatModel->insertRow(newRow);
    }
    // store the message in the model
    m_chatModel->setData(m_chatModel->index(newRow, 0), text);
    // set the alignment for the message
    m_chatModel->setData(m_chatModel->index(newRow, 0), int(Qt::AlignLeft | Qt::AlignVCenter), Qt::TextAlignmentRole);
    // scroll the view to display the new message
    ui->listView_chat->scrollToBottom();
}

void ClientWindow::sendMessage()
{
    // we use the client to send the message that the user typed
    m_client->sendMessage(ui->lineEdit_chat->text());
    // now we add the message to the list
    // store the index of the new row to append to the model containing the messages
    const int newRow = m_chatModel->rowCount();
    // insert a row for the message
    m_chatModel->insertRow(newRow);
    // store the message in the model
    m_chatModel->setData(m_chatModel->index(newRow, 0), ui->lineEdit_chat->text());
    // set the alignment for the message
    m_chatModel->setData(m_chatModel->index(newRow, 0), int(Qt::AlignRight | Qt::AlignVCenter), Qt::TextAlignmentRole);
    // clear the content of the message editor
    ui->lineEdit_chat->clear();
    // scroll the view to display the new message
    ui->listView_chat->scrollToBottom();
    // reset the last printed username
    m_lastUserName.clear();
}

void ClientWindow::disconnectedFromServer()
{
    // if the client loses connection to the server
    // comunicate the event to the user via a message box
    QMessageBox::warning(this, tr("Disconnected"), tr("The host terminated the connection"));
    // disable the ui to send and display messages
    ui->lineEdit_chat->setEnabled(false);
    ui->listView_chat->setEnabled(false);
    // reset the last printed username
    m_lastUserName.clear();
}

void ClientWindow::userJoined(const QString &username, const QStringList &allUserNames)
{
    // store the index of the new row to append to the model containing the messages
    const int newRow = m_chatModel->rowCount();
    // insert a row
    m_chatModel->insertRow(newRow);
    // store in the model the message to comunicate a user joined
    m_chatModel->setData(m_chatModel->index(newRow, 0), tr("%1 Joined the Chat").arg(username));
    // set the alignment for the text
    m_chatModel->setData(m_chatModel->index(newRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    // set the color for the text
    m_chatModel->setData(m_chatModel->index(newRow, 0), QBrush(Qt::blue), Qt::ForegroundRole);
    // scroll the view to display the new message
    ui->listView_chat->scrollToBottom();
    // reset the last printed username
    m_lastUserName.clear();

    // listview_player
    const int num_users = allUserNames.size();
    m_playerModel->clear();
    m_playerModel->insertColumn(0);
    m_playerModel->insertRows(0, num_users);
    for (int i = 0; i < num_users; ++i) {
        m_playerModel->setData(m_playerModel->index(i, 0), allUserNames[i]);
    }
}
void ClientWindow::userLeft(const QString &username)
{
    // store the index of the new row to append to the model containing the messages
    const int newRow = m_chatModel->rowCount();
    // insert a row
    m_chatModel->insertRow(newRow);
    // store in the model the message to comunicate a user left
    m_chatModel->setData(m_chatModel->index(newRow, 0), tr("%1 Left the Chat").arg(username));
    // set the alignment for the text
    m_chatModel->setData(m_chatModel->index(newRow, 0), Qt::AlignCenter, Qt::TextAlignmentRole);
    // set the color for the text
    m_chatModel->setData(m_chatModel->index(newRow, 0), QBrush(Qt::red), Qt::ForegroundRole);
    // scroll the view to display the new message
    ui->listView_chat->scrollToBottom();
    // reset the last printed username
    m_lastUserName.clear();

}

void ClientWindow::error(QAbstractSocket::SocketError socketError)
{
    // show a message to the user that informs of what kind of error occurred
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
    case QAbstractSocket::ProxyConnectionClosedError:
        return; // handled by disconnectedFromServer
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The host refused the connection"));
        break;
    case QAbstractSocket::ProxyConnectionRefusedError:
        QMessageBox::critical(this, tr("Error"), tr("The proxy refused the connection"));
        break;
    case QAbstractSocket::ProxyNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the proxy"));
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::critical(this, tr("Error"), tr("Could not find the server"));
        break;
    case QAbstractSocket::SocketAccessError:
        QMessageBox::critical(this, tr("Error"), tr("You don't have permissions to execute this operation"));
        break;
    case QAbstractSocket::SocketResourceError:
        QMessageBox::critical(this, tr("Error"), tr("Too many connections opened"));
        break;
    case QAbstractSocket::SocketTimeoutError:
        QMessageBox::warning(this, tr("Error"), tr("Operation timed out"));
        return;
    case QAbstractSocket::ProxyConnectionTimeoutError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy timed out"));
        break;
    case QAbstractSocket::NetworkError:
        QMessageBox::critical(this, tr("Error"), tr("Unable to reach the network"));
        break;
    case QAbstractSocket::UnknownSocketError:
        QMessageBox::critical(this, tr("Error"), tr("An unknown error occured"));
        break;
    case QAbstractSocket::UnsupportedSocketOperationError:
        QMessageBox::critical(this, tr("Error"), tr("Operation not supported"));
        break;
    case QAbstractSocket::ProxyAuthenticationRequiredError:
        QMessageBox::critical(this, tr("Error"), tr("Your proxy requires authentication"));
        break;
    case QAbstractSocket::ProxyProtocolError:
        QMessageBox::critical(this, tr("Error"), tr("Proxy comunication failed"));
        break;
    case QAbstractSocket::TemporaryError:
    case QAbstractSocket::OperationError:
        QMessageBox::warning(this, tr("Error"), tr("Operation failed, please try again"));
        return;
    default:
        Q_UNREACHABLE();
    }
    // disable the ui to send and display messages
    ui->lineEdit_chat->setEnabled(false);
    ui->listView_chat->setEnabled(false);
    // reset the last printed username
    m_lastUserName.clear();
}
