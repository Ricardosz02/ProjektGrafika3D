#ifndef WEAPON_H
#define WEAPON_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

// Typy obiektów do zebrania
const int WEAPON_PISTOL = 0;
const int WEAPON_SHOTGUN = 1;
const int AMMO_PISTOL_BOX = 2;
const int AMMO_SHOTGUN_BOX = 3;
const int ITEM_MEDKIT = 4; // NOWOŒÆ: Apteczka

struct Weapon
{
    float x, y;
    bool isCollected;
    float dist;
    int type; // Przechowuje typ obiektu
};

extern std::vector<Weapon> weapons;
extern int ammoPistol;
extern int ammoShotgun;
extern bool hasPistol;
extern bool hasShotgun;
extern int currentWeapon;

void initWeapons();
// ZMIANA: Dodano parametr int& health
void checkWeaponCollection(float playerX, float playerY, int& health);

#endif