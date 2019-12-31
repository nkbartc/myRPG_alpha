#ifndef BATTLE_H
#define BATTLE_H

#include "player.h"
#include "creature.h"

class Player;
class Creature;

class Battle {
  public:
    Battle();
    Battle(Player* player, QVector<Creature> creatures);
    void update();
    QString summarize();
    bool is_battle();
  private:
    int combat_;
    Player* player_;
    QVector<Creature> creatures_;
    QString battle_report_;
//    int player_speed_;
};

#endif // BATTLE_H
