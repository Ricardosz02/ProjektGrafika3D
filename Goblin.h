#ifndef GOBLIN_H
#define GOBLIN_H

#include <vector>

const float COLLISION_RADIUS = 0.5f;
const float GOBLIN_CHASE_SPEED = 0.02f;
const float FLYING_MONSTER_SPEED = 0.025f;
const float WALKING_MONSTER_SPEED = 0.03f; // Szybszy

// Typy potworów
const int MONSTER_TYPE_GOBLIN = 1;
const int MONSTER_TYPE_FLYING = 2;
const int MONSTER_TYPE_WALKING = 3; // NOWY POTWÓR

// Stany potwora
const int STATE_IDLE = 0;   // Chodzenie
const int STATE_ATTACK = 1; // Walka
const int STATE_PAIN = 2;   // Obrywanie/Śmierć

struct Sprite {
    float x;
    float y;
    int type; // 1, 2 lub 3
    float dist;
    bool isWeapon = false;

    int health = 100;
    bool isAlive = true;

    // AI & Animacja
    int state = STATE_IDLE;
    float stateTimer = 0.0f;     // Czas trwania bólu/ataku
    float attackCooldown = 0.0f; // Czas do zadania obrażeń

    // Zmienne dla Walkera
    float animTimer = 0.0f;      // Licznik do zmiany klatek
    int walkStep = 0;            // 0..3 (Cykl: 1,3,1,2)
    int fightFrame = 0;          // 0..1 (Cykl: Fight1, Fight2)
};

struct Fireball {
    float x, y;
    float dirX, dirY;
    bool active;
};

extern std::vector<Sprite> sprites;
extern std::vector<Fireball> fireballs;

void moveMonsters(float playerX, float playerY, float deltaTime, int& playerHealth);
bool checkCollision(float playerX, float playerY);
void initMonsters();
bool hitMonster(int index, float hitDamage);
void removeDeadMonsters();
void updateFireballs(float playerX, float playerY, float deltaTime, int& playerHealth);

#endif