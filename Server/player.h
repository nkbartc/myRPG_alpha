#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include <QVector>
#include <QMap>
#include <QSqlDatabase>
#include <QtSql>
#include "data_structs.h"
#include "creature.h"
//#include "item.h"
#include <QObject>

class Player {
public:
    Player();
    Player(const QJsonObject &json);
    void attack(Player* player);
    void take_damage(int dmg, QString creature_name);  // may not need this since status setter
    void set_cur_hp();  // this is called at begining and after a battle.
    void summarize();
    void ding();
    void equip();
    void use();
    void select_stage();
    void adventure();
    void rest();
    Status_player get_status();
    int cur_hp;
    QString combat_report_;
    void set_player_loc(QString loc_map, int loc_x, int loc_y);
signals:
    void notify(QString yee);
protected:
    Status_player status_;
    QSqlQuery qry_;
    int combat_;
//    QMap<QString, QVector<Gear>> gearBag_;
//    QMap<QString, QVector<Item>> itemBag_;
public slots:
    void attack(Creature &creature);
};

#endif // PLAYER_H
