#include "Weapon.h"
#include <cmath>
#include <iostream>

std::vector<Weapon> weapons;
int ammoPistol = 0;
int ammoShotgun = 0;
bool hasPistol = false;
bool hasShotgun = false;
int currentWeapon = 0; // 0 = Piêœci, 1 = Pistolet, 2 = Shotgun

void initWeapons() {
    weapons.clear();

    // Bronie
    weapons.push_back({ 3.5f, 9.5f, false, 0.0f, WEAPON_PISTOL });
    weapons.push_back({ 16.5f, 9.5f, false, 0.0f, WEAPON_SHOTGUN });

    // Amunicja
    weapons.push_back({ 10.5f, 9.5f, false, 0.0f, AMMO_PISTOL_BOX });
    weapons.push_back({ 10.5f, 10.5f, false, 0.0f, AMMO_SHOTGUN_BOX });

    // Apteczka
    weapons.push_back({ 11.5f, 10.5f, false, 0.0f, ITEM_MEDKIT });

    hasPistol = false;
    hasShotgun = false;
    currentWeapon = 0; // STARTUJEMY Z PIÊŒCIAMI
    ammoPistol = 0;
    ammoShotgun = 0;
    std::cout << "Bronie zainicjowane. Start z piesciami." << std::endl;
}

void checkWeaponCollection(float playerX, float playerY, int& health) {
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
                    // Nie zmieniamy automatu na pistolet, gracz sam musi wybraæ '2'
                    // chyba ¿e nie ma innej broni palnej, to dla wygody mo¿na zmieniæ:
                    if (currentWeapon == 0) currentWeapon = 1;
                    ammoPistol += 10;
                    std::cout << "ZEBRANO PISTOLET! (+10 Ammo)" << std::endl;
                }
                else if (w.type == WEAPON_SHOTGUN) {
                    hasShotgun = true;
                    if (currentWeapon == 0 || currentWeapon == 1) currentWeapon = 2;
                    ammoShotgun += 5;
                    std::cout << "ZEBRANO SHOTGUN! (+5 Ammo)" << std::endl;
                }
                else if (w.type == AMMO_PISTOL_BOX) {
                    ammoPistol += 10;
                }
                else if (w.type == AMMO_SHOTGUN_BOX) {
                    ammoShotgun += 5;
                }
                else if (w.type == ITEM_MEDKIT) {
                    health += 50;
                    if (health > 100) health = 100;
                }
            }
        }
    }
}