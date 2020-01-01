#include "creature.h"
#include <math.h>

Creature::Creature() {}

Creature::Creature(const QJsonObject &json, int lv){
    status_.lv = json.value("lv").toInt() * lv;
    status_.name = json.value("name").toString();
    status_.hp = json.value("hp").toInt() * lv;
    status_.atk = json.value("atk").toInt() * lv;
    status_.def = json.value("def").toInt() * lv;
    status_.mdef = json.value("mdef").toInt() * lv;
    status_.speed = json.value("speed").toInt() * lv;
    status_.dodge = json.value("dodge").toInt() * lv;
    status_.ac = json.value("ac").toInt() * lv;
    status_.crit = json.value("crit").toInt() * lv;
    status_.block = json.value("block").toInt() * lv;

    cur_hp = status_.hp;
}

void Creature::attack(Player* player, QString battle_report) {
    double pure_dmg = this->status_.atk;
    int player_def = player->get_status().def;
    int real_dmg = (int)round(pure_dmg);
    player->take_damage(real_dmg, status_.name, battle_report);
}

void Creature::take_damage(int dmg) {
    cur_hp -= dmg;
}

Status_creature Creature::get_status() {
    return status_;
}

QString Creature::get_name() {
    return status_.name;
}
