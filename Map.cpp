#include "Map.h"
#include "Weapon.h"
#include <iostream>

std::vector<Door> doors;
std::vector<WallDecal> wallDecals;

// LEGENDA MAPY:
// 1 - Œciana
// 2 - Drzwi Otwarte (bez karty)
// 3 - Drzwi na ZIELONA karte
// 4 - Drzwi na CZERWONA karte
// 5 - Drzwi na OBIE karty
// 9 - Portal
int worldMap1[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {9,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,2,1,1,1,3,1,1,1,4,1,1,1,1,1,5,1,1,1},
    {1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1},
    {1,0,0,0,1,0,0,0,1,0,0,0,0,0,1,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int worldMap2[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1},
    {1,0,1,1,1,0,0,1,0,0,0,0,0,1,0,1,0,0,0,1},
    {1,0,1,0,0,0,0,1,0,1,1,1,0,1,0,1,0,1,0,1},
    {1,0,1,0,1,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},
    {1,0,0,0,1,0,0,1,0,1,0,1,0,0,0,0,0,1,0,1},
    {1,1,1,1,1,0,0,1,0,1,0,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

int (*worldMap)[MAP_WIDTH] = worldMap1;

void initDoors() {
    doors.clear();
    wallDecals.clear();
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int t = worldMap[y][x];
            if (t >= 2 && t <= 5) {
                bool startLocked = (t != 2); // Tylko typ 2 jest od razu otwarty
                doors.push_back({ x, y, 0.0f, CLOSED, 0.0f, startLocked });
            }
        }
    }
}

void switchMap(int mapIndex) {
    if (mapIndex == 1) worldMap = worldMap1;
    else if (mapIndex == 2) worldMap = worldMap2;
    initDoors();
}

void updateDoors(float dt) {
    for (auto& door : doors) {
        switch (door.state) {
        case OPENING:
            door.openAmount += dt * 1.5f;
            if (door.openAmount >= 1.0f) {
                door.openAmount = 1.0f;
                door.state = OPEN;
                door.timer = 3.0f;
            }
            break;
        case OPEN:
            door.timer -= dt;
            if (door.timer <= 0.0f) door.state = CLOSING;
            break;
        case CLOSING:
            door.openAmount -= dt * 1.5f;
            if (door.openAmount <= 0.0f) {
                door.openAmount = 0.0f;
                door.state = CLOSED;
            }
            break;
        }
    }
}

void openDoorAt(int x, int y) {
    int doorType = worldMap[y][x];

    if (doorType == 3) {
        if (!hasGreenKey) {
            std::cout << "POTRZEBNA ZIELONA KARTA!" << std::endl;
            return;
        }
    }
    else if (doorType == 4) {
        if (!hasRedKey) {
            std::cout << "POTRZEBNA CZERWONA KARTA!" << std::endl;
            return;
        }
    }
    else if (doorType == 5) {
        if (!hasGreenKey || !hasRedKey) {
            std::cout << "POTRZEBNE OBIE KARTY!" << std::endl;
            return;
        }
    }
    else if (doorType != 2) {
        return;
    }

    for (auto& door : doors) {
        if (door.x == x && door.y == y) {
            if (door.state == CLOSED || door.state == CLOSING) {
                door.state = OPENING;
                door.isLocked = false; // ODBLOKOWUJEMY NA STALE!
                std::cout << "Otwieram drzwi!" << std::endl;
            }
            return;
        }
    }
}

Door* getDoor(int x, int y) {
    for (auto& door : doors) {
        if (door.x == x && door.y == y) return &door;
    }
    return nullptr;
}