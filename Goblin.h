#ifndef GOBLIN_H
#define GOBLIN_H

#include <vector>

const float COLLISION_RADIUS = 0.5f;
const float GOBLIN_CHASE_SPEED = 0.02f;
const float FLYING_MONSTER_SPEED = 0.025f;
const float WALKING_MONSTER_SPEED = 0.03f;

const int MONSTER_TYPE_GOBLIN = 1;
const int MONSTER_TYPE_FLYING = 2;
const int MONSTER_TYPE_WALKING = 3;

const int OBJECT_HOLE_PISTOL = 98;
const int OBJECT_HOLE_SHOTGUN = 99;
const int OBJECT_BLOOD = 80;

const int STATE_IDLE = 0;
const int STATE_ATTACK = 1;
const int STATE_PAIN = 2;

struct Sprite {
    float x, y;
    int type;
    float dist;
    bool isWeapon = false;

    int health = 100;
    bool isAlive = true;

    int state = STATE_IDLE;
    float stateTimer = 0.0f;
    float attackCooldown = 0.0f;

    float soundTimer = 0.0f;

    float animTimer = 0.0f;
    int walkStep = 0;
    int fightFrame = 0;

    float zOffset = 0.0f;
};

struct BloodParticle {
    float x, y, z;
    float velX, velY, velZ;
    float life;
};

struct Fireball {
    float x, y;
    float dirX, dirY;
    bool active;
};

extern std::vector<Sprite> sprites;
extern std::vector<Fireball> fireballs;
extern std::vector<BloodParticle> bloodParticles;

void moveMonsters(float playerX, float playerY, float deltaTime, int& playerHealth, int& playerArmor, float& damageAlpha);
bool checkCollision(float playerX, float playerY);
void initMonsters();
bool hitMonster(int index, float hitDamage);
void removeDeadMonsters();
void updateFireballs(float playerX, float playerY, float deltaTime, int& playerHealth, int& playerArmor, float& damageAlpha);
void updateBloodParticles(float dt);

#endif