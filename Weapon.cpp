#include "Weapon.h"
#include <cmath>
#include <iostream>

std::vector<Weapon> weapons;
int ammoPistol = 0;
int ammoShotgun = 0;
bool hasPistol = false;
bool hasShotgun = false;
int currentWeapon = 0; // 0 = Brak, 1 = Pistolet, 2 = Shotgun

void initWeapons() {
    weapons.clear();
    weapons.push_back({ 3.5f, 9.5f, false, 0.0f, WEAPON_PISTOL }); //PISTOL
    weapons.push_back({ 16.5f, 9.5f, false, 0.0f, WEAPON_SHOTGUN }); //SHOTGUN

    hasPistol = false;
    hasShotgun = false;
    currentWeapon = 0;
    ammoPistol = 0;
    ammoShotgun = 0;
    std::cout << "Bron zainicjowana." << std::endl;
}

void checkWeaponCollection(float playerX, float playerY) {
    float collectionDistance = 0.5f;

    for (auto& w : weapons) {
        if (!w.isCollected) {
            float dx = w.x - playerX;
            float dy = w.y - playerY;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < collectionDistance) {
                w.isCollected = true;

                if (w.type == WEAPON_PISTOL) {
                    hasPistol = true;
                    if (currentWeapon == 0) currentWeapon = 1;

                    ammoPistol += 10;
                    std::cout << "ZEBRANO PISTOLET! Amunicja Pistoletu: " << ammoPistol << std::endl;
                }
                else if (w.type == WEAPON_SHOTGUN) {
                    hasShotgun = true;
                    currentWeapon = 2;

                    ammoShotgun += 5;
                    std::cout << "ZEBRANO SHOTGUN! Amunicja Shotguna: " << ammoShotgun << std::endl;
                }
            }
        }
    }
}