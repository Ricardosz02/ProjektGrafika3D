#include "Weapon.h"
#include "Audio.h"
#include <cmath>
#include <iostream>

std::vector<Weapon> weapons;
int ammoPistol = 0;
int ammoShotgun = 0;
int ammoRifle = 0;

bool hasPistol = false;
bool hasShotgun = false;
bool hasRifle = false;

int currentWeapon = 0;

bool hasGreenKey = false;
bool hasRedKey = false;

void initWeapons() {
    weapons.clear();

    weapons.push_back({ 3.5f, 9.5f, false, 0.0f, WEAPON_PISTOL });
    weapons.push_back({ 16.5f, 9.5f, false, 0.0f, WEAPON_SHOTGUN });
    weapons.push_back({ 13.5f, 7.5f, false, 0.0f, WEAPON_RIFLE });

    weapons.push_back({ 10.5f, 9.5f, false, 0.0f, AMMO_PISTOL_BOX });
    weapons.push_back({ 10.5f, 10.5f, false, 0.0f, AMMO_SHOTGUN_BOX });
    weapons.push_back({ 14.5f, 7.5f, false, 0.0f, AMMO_RIFLE_BOX });

    weapons.push_back({ 11.5f, 10.5f, false, 0.0f, ITEM_MEDKIT });
    weapons.push_back({ 12.5f, 10.5f, false, 0.0f, ITEM_ARMOR });

    weapons.push_back({ 5.5f, 5.5f, false, 0.0f, ITEM_KEY_GREEN });
    weapons.push_back({ 15.5f, 15.5f, false, 0.0f, ITEM_KEY_RED });

    hasPistol = false;
    hasShotgun = false;
    hasRifle = false;

    hasGreenKey = false;
    hasRedKey = false;

    currentWeapon = 0;
    ammoPistol = 0;
    ammoShotgun = 0;
    ammoRifle = 0;

    std::cout << "Inicjalizacja przedmiotow zakonczona." << std::endl;
}

void checkWeaponCollection(float playerX, float playerY, int& health, int& armor) {
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
                    playPickupSound(PICKUP_WEAPON);
                    std::cout << "ZEBRANO PISTOLET! (+10 Ammo)" << std::endl;
                }
                else if (w.type == WEAPON_SHOTGUN) {
                    hasShotgun = true;
                    if (currentWeapon == 0 || currentWeapon == 1) currentWeapon = 2;
                    ammoShotgun += 5;
                    playPickupSound(PICKUP_WEAPON);
                    std::cout << "ZEBRANO SHOTGUN! (+5 Ammo)" << std::endl;
                }
                else if (w.type == WEAPON_RIFLE) {
                    hasRifle = true;
                    currentWeapon = 3;
                    ammoRifle += 50;
                    playPickupSound(PICKUP_WEAPON);
                    std::cout << "ZEBRANO KARABIN! (+50 Ammo)" << std::endl;
                }
                else if (w.type == AMMO_PISTOL_BOX) {
                    ammoPistol += 10;
                    playPickupSound(PICKUP_AMMO);
                    std::cout << "Amunicja Pistoletu (+10)" << std::endl;
                }
                else if (w.type == AMMO_SHOTGUN_BOX) {
                    ammoShotgun += 5;
                    playPickupSound(PICKUP_AMMO);
                    std::cout << "Amunicja Shotguna (+5)" << std::endl;
                }
                else if (w.type == AMMO_RIFLE_BOX) {
                    ammoRifle += 20;
                    playPickupSound(PICKUP_AMMO);
                    std::cout << "Amunicja Karabinu (+20)" << std::endl;
                }
                else if (w.type == ITEM_MEDKIT) {
                    health += 50;
                    if (health > 100) health = 100;
                    playPickupSound(PICKUP_HEALTH);
                    std::cout << "Apteczka (+50 HP)" << std::endl;
                }
                else if (w.type == ITEM_ARMOR) {
                    armor += 100;
                    if (armor > 100) armor = 100;
                    playPickupSound(PICKUP_ARMOR);
                    std::cout << "KAMIZELKA (+50 ARMOR)" << std::endl;
                }
                else if (w.type == ITEM_KEY_GREEN) {
                    hasGreenKey = true;
                    playPickupSound(PICKUP_KEY);
                    std::cout << "ZEBRANO ZIELONA KARTE!" << std::endl;
                }
                else if (w.type == ITEM_KEY_RED) {
                    hasRedKey = true;
                    playPickupSound(PICKUP_KEY);
                    std::cout << "ZEBRANO CZERWONA KARTE!" << std::endl;
                }
            }
        }
    }
}