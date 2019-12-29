//#include "creature.h"
//#include <math.h>

//Creature::Creature() {}

//Creature::Creature(int id, int lv){
//    qry_.exec("select * from Creature where Id='"+ QString::number(id) + "'");
//    qry_.next();
//    status_.id = qry_.value("Id").toInt();
//    status_.name = qry_.value("Name").toString();
//    status_.lv = lv;
//    status_.hp = qry_.value("Hp").toInt() * lv;
//    status_.atk = qry_.value("Atk").toInt() * lv;
//    status_.def = qry_.value("Def").toInt() * lv;
//    status_.mdef = qry_.value("MDef").toInt() * lv;
//    status_.speed = qry_.value("Speed").toInt() * lv;
//    status_.dodge = qry_.value("Dodge").toInt() * lv;
//    status_.ac = qry_.value("Ac").toInt() * lv;
//    status_.crit = qry_.value("Crit").toInt() * lv;
//    status_.block = qry_.value("Block").toInt() * lv;

//    cur_hp = status_.hp;
//}

//void Creature::attack(Player* player) {
//    double pure_dmg = this->status_.atk;
//    int player_def = player->get_status().def;
//    int real_dmg = (int)round(pure_dmg);
//    player->take_damage(real_dmg, status_.name);
//}

//void Creature::take_damage(int dmg) {
//    cur_hp -= dmg;
//}

//Status_creature Creature::get_status() {
//    return status_;
//}

//QString Creature::get_name() {
//    return status_.name;
//}
