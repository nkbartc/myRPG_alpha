#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <QString>
#include <QPair>
#include <iostream>
//#include "gear.h"

typedef QPair<QString, QPair<int,int>> Location; // <map,<x,y>>

struct Status_player {
  int userId;
  int characterId;
  QString class_;
  QString name;
  QString time;
  int lv;
  int exp;
  int reqExp;
  int fame;
  int gold;
  int diamond;
  int hp;
  int atk;
  int def;
  int mdef;
  int speed;
  int dodge;
  int ac;
  int crit;
  int block;
  Location location;
//  Gear mainhand;
//  Gear offhand;
//  Gear head;
//  Gear armor;
//  Gear feet;
//  Gear ring1;
//  Gear ring2;
//  Gear trinket1;
//  Gear trinket2;
//  Gear pendant;
};

struct Status_creature {
  int lv;
  QString name;
//  QString category;
  int hp;
  int atk;
  int def;
  int mdef;
  int speed;
  int dodge;
  int ac;
  int crit;
  int block;
};

#endif // DATA_STRUCTS_H
