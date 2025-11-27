#ifndef WEAPON_H
#define WEAPON_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

// Typy broni
const int WEAPON_PISTOL = 0;
const int WEAPON_SHOTGUN = 1;

struct Weapon
{
    float x, y;
    bool isCollected;
    float dist;
    int type; // 0 = Pistolet, 1 = Shotgun
};

extern std::vector<Weapon> weapons;
extern int ammoPistol;
extern int ammoShotgun;
extern bool hasPistol;
extern bool hasShotgun;
extern int currentWeapon; // 1 = Pistolet, 2 = Shotgun

void initWeapons();
void checkWeaponCollection(float playerX, float playerY);

#endif