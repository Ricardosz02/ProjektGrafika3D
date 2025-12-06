#pragma once
#include <vector>

#define MAP_WIDTH 20
#define MAP_HEIGHT 20

enum DoorState { CLOSED, OPENING, OPEN, CLOSING };

struct Door {
    int x, y;
    float openAmount;
    DoorState state;
    float timer;
    bool isLocked; // NOWE: Czy drzwi s¹ zaryglowane?
};

struct WallDecal {
    int x, y;
    int side;
    float hitX;
    float hitY;
    int type;
};

extern int worldMap1[MAP_HEIGHT][MAP_WIDTH];
extern int worldMap2[MAP_HEIGHT][MAP_WIDTH];

extern int (*worldMap)[MAP_WIDTH];
extern std::vector<Door> doors;
extern std::vector<WallDecal> wallDecals;

void switchMap(int mapIndex);
void initDoors();
void updateDoors(float dt);
void openDoorAt(int x, int y);
Door* getDoor(int x, int y);