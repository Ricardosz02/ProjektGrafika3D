#include "Weapon.h"
#include <cmath>
#include <iostream>

std::vector<Weapon> weapons;
int ammoPistol = 0;
int ammoShotgun = 0;
bool hasPistol = false;
bool hasShotgun = false;
int currentWeapon = 0;

void initWeapons() {
    weapons.clear();

    // Bronie (tak jak by³y)
    weapons.push_back({ 3.5f, 9.5f, false, 0.0f, WEAPON_PISTOL });
    weapons.push_back({ 16.5f, 9.5f, false, 0.0f, WEAPON_SHOTGUN });

    // --- NOWA AMUNICJA (ŒRODEK MAPY) ---
    // Amunicja do pistoletu na (10.5, 9.5)
    weapons.push_back({ 10.5f, 9.5f, false, 0.0f, AMMO_PISTOL_BOX });

    // Amunicja do shotguna na (10.5, 10.5)
    weapons.push_back({ 10.5f, 10.5f, false, 0.0f, AMMO_SHOTGUN_BOX });

    hasPistol = false;
    hasShotgun = false;
    currentWeapon = 0;
    ammoPistol = 0;
    ammoShotgun = 0;
    std::cout << "Bronie i amunicja rozmieszczone." << std::endl;
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

                // Logika dla broni
                if (w.type == WEAPON_PISTOL) {
                    hasPistol = true;
                    if (currentWeapon == 0) currentWeapon = 1;
                    ammoPistol += 10;
                    std::cout << "ZEBRANO PISTOLET! (+10 Ammo)" << std::endl;
                }
                else if (w.type == WEAPON_SHOTGUN) {
                    hasShotgun = true;
                    currentWeapon = 2;
                    ammoShotgun += 5;
                    std::cout << "ZEBRANO SHOTGUN! (+5 Ammo)" << std::endl;
                }
                // Logika dla amunicji
                else if (w.type == AMMO_PISTOL_BOX) {
                    ammoPistol += 10;
                    std::cout << "ZEBRANO AMUNICJE PISTOLETU (+10)" << std::endl;
                }
                else if (w.type == AMMO_SHOTGUN_BOX) {
                    ammoShotgun += 5;
                    std::cout << "ZEBRANO AMUNICJE SHOTGUNA (+5)" << std::endl;
                }
            }
        }
    }
}