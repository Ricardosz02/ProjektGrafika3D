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
bool hasSecretNote = false;

extern int activeMapIndex;

void initWeapons() {
    weapons.clear();
    /*
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

    weapons.push_back({ 3.5f, 3.5f, false, 0.0f, WEAPON_TYPE_NOTE });
    */

    if (activeMapIndex == 1) {

        weapons.push_back({ 15.0f, 58.0f, false, 0.0f, WEAPON_PISTOL });

        weapons.push_back({ 19.0f, 38.0f, false, 0.0f, AMMO_PISTOL_BOX });

        weapons.push_back({ 35.0f, 53.0f, false, 0.0f, ITEM_MEDKIT });

        std::cout << "Dodano przedmioty dla Mapy 1.\n";
    }

    else if (activeMapIndex == 2) {

        weapons.push_back({ 9.0f, 36.0f, false, 0.0f, AMMO_PISTOL_BOX });
        weapons.push_back({ 2.0f, 8.0f, false, 0.0f, AMMO_PISTOL_BOX });
        weapons.push_back({ 3.0f, 32.0f, false, 0.0f, AMMO_PISTOL_BOX });
        weapons.push_back({ 2.0f, 6.0f, false, 0.0f, AMMO_PISTOL_BOX });

        weapons.push_back({ 32.0f, 3.0f, false, 0.0f, ITEM_KEY_GREEN });

        weapons.push_back({ 21.0f, 25.0f, false, 0.0f, ITEM_MEDKIT });
        weapons.push_back({ 46.0f, 23.5f, false, 0.0f, ITEM_MEDKIT });

        weapons.push_back({ 2.0f, 8.0f, false, 0.0f, ITEM_ARMOR });
        weapons.push_back({ 13.0f, 36.0f, false, 0.0f, ITEM_ARMOR });

        std::cout << "Dodano przedmioty dla Mapy 1.\n";

    }

    else if (activeMapIndex == 3) {

        weapons.push_back({ 6.0f, 13.0f, false, 0.0f, WEAPON_SHOTGUN });

        weapons.push_back({ 22.0f, 15.0f, false, 0.0f, AMMO_SHOTGUN_BOX });
        weapons.push_back({ 25.0f, 15.0f, false, 0.0f, AMMO_SHOTGUN_BOX });

        weapons.push_back({ 3.5f, 52.0f, false, 0.0f, AMMO_PISTOL_BOX });
        weapons.push_back({ 30.5f, 52.5f, false, 0.0f, AMMO_PISTOL_BOX });

        weapons.push_back({ 27.5f, 45.5f, false, 0.0f, ITEM_KEY_GREEN });

        weapons.push_back({ 23.5f, 14.0f, false, 0.0f, ITEM_KEY_RED });

        std::cout << "Dodano przedmioty dla Mapy 3.\n";

    }

    else if (activeMapIndex == 4) {

        weapons.push_back({ 4.0f, 48.0f, false, 0.0f, WEAPON_RIFLE });

        weapons.push_back({ 24.0f, 36.0f, false, 0.0f, ITEM_KEY_GREEN });

        weapons.push_back({ 44.0f, 16.0f, false, 0.0f, ITEM_KEY_RED });

        weapons.push_back({ 24.0f, 4.0f, false, 0.0f, AMMO_SHOTGUN_BOX });
        weapons.push_back({ 20.0f, 28.0f, false, 0.0f, AMMO_SHOTGUN_BOX });

        weapons.push_back({ 28.0f, 12.0f, false, 0.0f, AMMO_PISTOL_BOX });
        weapons.push_back({ 36.0f, 36.0f, false, 0.0f, AMMO_PISTOL_BOX });

        weapons.push_back({ 48.0f, 27.0f, false, 0.0f, AMMO_RIFLE_BOX });

        std::cout << "Dodano przedmioty dla Mapy 4.\n";

    }

    else if (activeMapIndex == 5) {

        weapons.push_back({ 3.0f, 54.5f, false, 0.0f, ITEM_KEY_RED });

        weapons.push_back({ 52.0f, 52.5f, false, 0.0f, WEAPON_TYPE_NOTE });

        weapons.push_back({ 9.0f, 53.0f, false, 0.0f, AMMO_RIFLE_BOX });
        weapons.push_back({ 45.0f, 58.0f, false, 0.0f, AMMO_RIFLE_BOX });

        weapons.push_back({ 39.0f, 52.5f, false, 0.0f, AMMO_SHOTGUN_BOX });

        weapons.push_back({ 45.0f, 52.5f, false, 0.0f, ITEM_ARMOR });
        weapons.push_back({ 52.0f, 41.5f, false, 0.0f, ITEM_ARMOR });

        weapons.push_back({ 4.5f, 41.0f, false, 0.0f, ITEM_MEDKIT });

        std::cout << "Dodano przedmioty dla Mapy 5.\n";

    }

    else if (activeMapIndex == 6) {

        weapons.push_back({ 8.0f, 30.0f, false, 0.0f, ITEM_MEDKIT });

        weapons.push_back({ 8.0f, 29.0f, false, 0.0f, ITEM_ARMOR });
        weapons.push_back({ 8.0f, 31.0f, false, 0.0f, ITEM_ARMOR });

        weapons.push_back({ 8.0f, 39.0f, false, 0.0f, AMMO_PISTOL_BOX });
        weapons.push_back({ 8.0f, 41.0f, false, 0.0f, AMMO_PISTOL_BOX });
        weapons.push_back({ 8.0f, 43.0f, false, 0.0f, AMMO_PISTOL_BOX });
        weapons.push_back({ 8.0f, 45.0f, false, 0.0f, AMMO_PISTOL_BOX });
        weapons.push_back({ 8.0f, 47.0f, false, 0.0f, AMMO_PISTOL_BOX });
        weapons.push_back({ 8.0f, 49.0f, false, 0.0f, AMMO_PISTOL_BOX });

        weapons.push_back({ 8.0f, 21.0f, false, 0.0f, AMMO_SHOTGUN_BOX });
        weapons.push_back({ 8.0f, 19.0f, false, 0.0f, AMMO_SHOTGUN_BOX });
        weapons.push_back({ 8.0f, 17.0f, false, 0.0f, AMMO_SHOTGUN_BOX });
        weapons.push_back({ 8.0f, 15.0f, false, 0.0f, AMMO_SHOTGUN_BOX });
        weapons.push_back({ 8.0f, 13.0f, false, 0.0f, AMMO_SHOTGUN_BOX });
        weapons.push_back({ 8.0f, 11.0f, false, 0.0f, AMMO_SHOTGUN_BOX });

        weapons.push_back({ 19.0f, 30.0f, false, 0.0f, AMMO_RIFLE_BOX });
        weapons.push_back({ 15.0f, 30.0f, false, 0.0f, AMMO_RIFLE_BOX });
        weapons.push_back({ 23.0f, 30.0f, false, 0.0f, AMMO_RIFLE_BOX });

        weapons.push_back({ 31.0f, 21.0f, false, 0.0f, ITEM_ARMOR });
        weapons.push_back({ 31.0f, 39.0f, false, 0.0f, ITEM_ARMOR });

        weapons.push_back({ 49.0f, 21.0f, false, 0.0f, ITEM_MEDKIT });
        weapons.push_back({ 49.0f, 39.0f, false, 0.0f, ITEM_MEDKIT });


        std::cout << "Dodano przedmioty dla Mapy 6.\n";

    }

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
                    ammoPistol += 20;
                    playPickupSound(PICKUP_WEAPON);
                    std::cout << "ZEBRANO PISTOLET! (+20 Ammo)" << std::endl;
                }
                else if (w.type == WEAPON_SHOTGUN) {
                    hasShotgun = true;
                    if (currentWeapon == 0 || currentWeapon == 1) currentWeapon = 2;
                    ammoShotgun += 10;
                    playPickupSound(PICKUP_WEAPON);
                    std::cout << "ZEBRANO SHOTGUN! (+10 Ammo)" << std::endl;
                }
                else if (w.type == WEAPON_RIFLE) {
                    hasRifle = true;
                    currentWeapon = 3;
                    ammoRifle += 50;
                    playPickupSound(PICKUP_WEAPON);
                    std::cout << "ZEBRANO KARABIN! (+50 Ammo)" << std::endl;
                }
                else if (w.type == AMMO_PISTOL_BOX) {
                    ammoPistol += 20;
                    playPickupSound(PICKUP_AMMO);
                    std::cout << "Amunicja Pistoletu (+20)" << std::endl;
                }
                else if (w.type == AMMO_SHOTGUN_BOX) {
                    ammoShotgun += 10;
                    playPickupSound(PICKUP_AMMO);
                    std::cout << "Amunicja Shotguna (+10)" << std::endl;
                }
                else if (w.type == AMMO_RIFLE_BOX) {
                    ammoRifle += 25;
                    playPickupSound(PICKUP_AMMO);
                    std::cout << "Amunicja Karabinu (+25)" << std::endl;
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
                else if (w.type == WEAPON_TYPE_NOTE) {
                    hasSecretNote = true;
                    playPickupSound(PICKUP_KEY);
                    std::cout << "Podniesiono notatke! Nacisnij 'N' aby przeczytac." << std::endl;
                }
            }
        }
    }
}

void resetKeys() {
    hasGreenKey = false;
    hasRedKey = false;
    hasSecretNote = false;
    std::cout << "Klucze i kartki zostaly usuniete!" << std::endl;
}