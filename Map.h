#pragma once
#include <vector>

#define MAP_WIDTH 60
#define MAP_HEIGHT 60

enum DoorState { CLOSED, OPENING, OPEN, CLOSING };

struct Door {
    int x, y;
    float openAmount;
    DoorState state;
    float timer;
    bool isLocked;
};

struct WallDecal {
    int x, y;
    int side;
    float hitX;
    float hitY;
    int type;
};

extern bool isKeypadActive;
extern bool isKeypadSuccess;
extern float keypadSuccessTimer;
extern Door* targetKeypadDoor;

extern int worldMap1[MAP_HEIGHT][MAP_WIDTH];
extern int worldMap2[MAP_HEIGHT][MAP_WIDTH];
extern int worldMap3[MAP_HEIGHT][MAP_WIDTH];
extern int worldMap4[MAP_HEIGHT][MAP_WIDTH];
extern int worldMap5[MAP_HEIGHT][MAP_WIDTH];
extern int worldMap6[MAP_HEIGHT][MAP_WIDTH];

extern int (*worldMap)[MAP_WIDTH];
extern std::vector<Door> doors;
extern std::vector<WallDecal> wallDecals;

void switchMap(int mapIndex);
void initDoors();
void updateDoors(float dt);
void openDoorAt(int x, int y);
Door* getDoor(int x, int y);