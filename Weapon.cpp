#include "Weapon.h"
#include <cmath>
#include <iostream>

std::vector<Weapon> weapons;
int ammoCount = 0;
bool hasPistol = false;

void initWeapons() {
    weapons.clear();
    weapons.push_back({ 3.5f, 9.5f, false, 0.0f });

    hasPistol = false;
    ammoCount = 0;
    std::cout << "Bron zainicjowana na (3.5, 9.5)." << std::endl;
}

void checkWeaponCollection(float playerX, float playerY) {
    if (hasPistol) return;

    float collectionDistance = 0.5f;

    for (auto& w : weapons) {
        if (!w.isCollected) {
            float dx = w.x - playerX;
            float dy = w.y - playerY;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < collectionDistance) {
                w.isCollected = true;
                hasPistol = true;
                ammoCount = 10;
                std::cout << "ZEBRANO PISTOLET! Amunicja: 10." << std::endl;
            }
        }
    }
}