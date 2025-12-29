#ifndef WEAPON_H
#define WEAPON_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

const int WEAPON_PISTOL = 0;
const int WEAPON_SHOTGUN = 1;
const int WEAPON_RIFLE = 8;
const int AMMO_PISTOL_BOX = 2;
const int AMMO_SHOTGUN_BOX = 3;
const int AMMO_RIFLE_BOX = 9;
const int ITEM_MEDKIT = 4;
const int ITEM_ARMOR = 5;

const int ITEM_KEY_GREEN = 6;
const int ITEM_KEY_RED = 7;

const int WEAPON_TYPE_NOTE = 10;

struct Weapon
{
    float x, y;
    bool isCollected;
    float dist;
    int type;
};

extern std::vector<Weapon> weapons;
extern int ammoPistol;
extern int ammoShotgun;
extern int ammoRifle;
extern bool hasRifle;
extern bool hasPistol;
extern bool hasShotgun;
extern int currentWeapon;

extern bool hasGreenKey;
extern bool hasRedKey;

extern bool hasSecretNote;

void initWeapons();
void checkWeaponCollection(float playerX, float playerY, int& health, int& armor);

#endif