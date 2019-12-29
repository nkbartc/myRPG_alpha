#ifndef CREATURE_H
#define CREATURE_H

#include <iostream>
#include <QVector>
#include <QSqlDatabase>
#include <QtSql>
#include "data_structs.h"
#include "player.h"
#include <QString>

class Player;

class Creature {
  public:
    Creature();
    Creature(int id, int lv);
    void attack(Player* player);
    void take_damage(int dmg);
    Status_creature get_status();
    QString get_name();
    int cur_hp;

  protected:
    Status_creature status_;
    QSqlQuery qry_;
};

#endif // CREATURE_H
