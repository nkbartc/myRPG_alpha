#include "battle.h"
Battle::Battle(){}

Battle::Battle(Player* player, QVector<Creature> creatures) {
  player_ = player;
  player->set_cur_hp();
  creatures_ = creatures;
  combat_ = 1;
  battle_report_ += "New battle created.\n";

  QString creature_name;
  for (auto it = creatures_.begin(); it != creatures_.end(); ++it) {
    creature_name += it->get_name() + "(" + QString::number(it->cur_hp) + "), ";
  }
  battle_report_ += creature_name + "\n";
}

void Battle::update() {
  // PVE player always goes first.
  for (auto it = creatures_.begin(); it != creatures_.end(); ++it) {
    if (it->cur_hp > 0) {
       player_->attack(*it, battle_report_);
       break;
    }
  }

  // enemies' turn.
  for (auto it = creatures_.begin(); it != creatures_.end();) {
    if (it->cur_hp <= 0) {
      battle_report_ += it->get_name() + " is dead.\n";
      it = creatures_.erase(it);
    } else {
      it->attack(player_, battle_report_);
      if (player_->cur_hp <= 0) {
        // player loses.
        combat_ = 0;
        player_->set_cur_hp();
        battle_report_ += "Battle ends. Player losses.\n";
        return;
      }
      ++it;
    }
  }
  // all enemies are dead. battle ends.
  if (creatures_.empty()) {
    combat_ = 0;
    player_->set_cur_hp();
    battle_report_ += "Battle ends. Player wins.\n";
  }
}

QString Battle::summarize() {
    return battle_report_;
}

bool Battle::is_battle() {
    return combat_;
}
