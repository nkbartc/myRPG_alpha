#ifndef CLIENTWINDOW_H
#define CLIENTWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QSqlDatabase>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QtSql>
#include <QScrollBar>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QInputDialog>
#include "client.h"
#include <QStandardItemModel>
#include <QMessageBox>
#include <cmath>
#include <QAction>
#include <QToolBar>
#include <QMenu>


QT_BEGIN_NAMESPACE
namespace Ui { class ClientWindow; }
QT_END_NAMESPACE

class Client;
class QStandardItemModel;

class ClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    ClientWindow(QWidget *parent = nullptr);
    ~ClientWindow();
    void RunWorld();
    void update();
    void update_status_client();
    void update_bag_client();
    void update_map();
    void update_stage(QString selected_map);
    void setup_tiled_map();
//    void move_click();
    bool eventFilter(QObject *target, QEvent *event);
    // need more update functions (chat, more bag, etc)

public slots:
    void update_combat_log(QString report);

private slots:
    // socket, login, and chat
    void attemptConnection();
    void connectedToServer();
    void signUp();
    void attemptSignUp(const QList<QLineEdit *> &fields);
    void attemptLogin(const QList<QLineEdit *> &fields);
    void loggedIn();
    void loginFailed(const QString &reason);
    void signedUp();
    void signUpFailed(const QString &reason);
    void messageReceived(const QString &sender, const QString &text);
    void sendMessage();
    void disconnectedFromServer();
    void userJoined(const QString &username, const QStringList &allUserNames);
    void userLeft(const QString &username);
    void error(QAbstractSocket::SocketError socketError);

    // others
//    void on_pushButton_move_clicked();
    void on_pushButton_clear_combat_clicked();
    void getPlayerStat(const QJsonObject &json);
    void getPlayerLoc(const QString loc_x, const QString loc_y);
    void getBattleReport(const QString battleReport);
    void setupMap(const QString map);

private:
    void setupBackground();
    void setupToolBar();
    Ui::ClientWindow *ui;
    Client *m_client;
    QStandardItemModel *m_chatModel;
    QStandardItemModel *m_playerModel;
    QString m_lastUserName;

    int cur_x;
    int cur_y;

    QSqlQuery qry_;
    QGraphicsScene *scene;
    QPixmap srcImage;
    QPointF mouse_pos_map;
};
#endif // CLIENTWINDOW_H
