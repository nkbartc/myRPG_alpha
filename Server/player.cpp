#include "player.h"
#include <math.h>

Player::Player() {}

Player::Player(const QJsonObject &json) {
    // json = {"type":"charInfo",
    //         "status":{status}
    //        }
    QJsonValue status = json.value(QLatin1String("status"));
    status_.userId = status.toObject().value("userId").toInt();
    status_.characterId = status.toObject().value("CharacterId").toInt();
    status_.class_ = status.toObject().value("Class").toString();
    status_.name = status.toObject().value("Name").toString();
    //  status_.time
    status_.lv = status.toObject().value("Lv").toInt();
    status_.exp = status.toObject().value("Exp").toInt();
    status_.reqExp = status.toObject().value("Req_Exp").toInt();
    status_.fame = status.toObject().value("Fame").toInt();
    status_.gold = status.toObject().value("Gold").toInt();
    status_.diamond = status.toObject().value("Diamond").toInt();
    status_.hp = status.toObject().value("Hp").toInt();
    status_.atk = status.toObject().value("Atk").toInt();
    status_.def = status.toObject().value("Def").toInt();
    status_.mdef = status.toObject().value("MDef").toInt();
    status_.speed = status.toObject().value("Speed").toInt();
    status_.dodge = status.toObject().value("Dodge").toInt();
    status_.ac = status.toObject().value("Ac").toInt();
    status_.crit = status.toObject().value("Crit").toInt();
    status_.block = status.toObject().value("Block").toInt();
    // status_gears implement them all later
}

//void Player::attack(Creature &creature) {
//    double pure_dmg = this->status_.atk;
//    int creature_def = creature.get_status().def;
//    //  int real_dmg = (int)round(pure_dmg);
//    int real_dmg = 5;
//    creature.take_damage(real_dmg);
//    QString mob_name = creature.get_name();
//    combat_report_ += "Player causes " + QString::number(real_dmg) + " damages to " +
//          mob_name + "(" + QString::number(creature.cur_hp) + ")\n";
//    emit notify(combat_report_);
//}

//void Player::attack(Player* player) {

//}

//void Player::take_damage(int dmg, QString creature_name) {
//    cur_hp -= dmg;
//    combat_report_+= "Player(" + QString::number(cur_hp) + ") takes " + QString::number(dmg) +
//                   " damages from " + creature_name +"\n" ;
//}

//void Player::set_cur_hp() {
//    cur_hp = status_.hp;
//}

//void Player::ding() {

//}

//Status_player Player::get_status() {
//    return status_;
//}