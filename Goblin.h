#ifndef GOBLIN_H
#define GOBLIN_H

#include <vector>

const float COLLISION_RADIUS = 0.5f;
const float GOBLIN_CHASE_SPEED = 0.02f;

const int MONSTER_TYPE_GOBLIN = 1;

struct Sprite {
    float x;
    float y;
    int type;
    float dist;
    bool isWeapon = false;

    int health = 100;
    bool isAlive = true;
};

extern std::vector<Sprite> sprites;

void moveGoblin(float playerX, float playerY);
bool checkCollision(float playerX, float playerY);
void initGoblin(float startX, float startY);
bool hitGoblin(float hitDamage);
void removeDeadGoblins();

#endif