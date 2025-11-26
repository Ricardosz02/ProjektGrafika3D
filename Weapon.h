#ifndef WEAPON_H
#define WEAPON_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

struct Weapon
{
    float x, y;
    bool isCollected;
    float dist;
};

extern std::vector<Weapon> weapons;
extern int ammoCount;
extern bool hasPistol;

void initWeapons();
void checkWeaponCollection(float playerX, float playerY);

#endif