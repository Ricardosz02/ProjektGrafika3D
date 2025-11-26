#pragma once

#define MAP_WIDTH 20
#define MAP_HEIGHT 20

extern int worldMap1[MAP_HEIGHT][MAP_WIDTH];
extern int worldMap2[MAP_HEIGHT][MAP_WIDTH];

extern int (*worldMap)[MAP_WIDTH];

void switchMap(int mapIndex);