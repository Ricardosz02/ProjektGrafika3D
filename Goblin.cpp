#include "Goblin.h" 
#include <vector>
#include <cmath> 
#include <iostream>
#include <algorithm> 

std::vector<Sprite> sprites;

void initGoblin(float startX, float startY) {
    sprites.clear();

    Sprite newGoblin;
    newGoblin.x = startX;
    newGoblin.y = startY;
    newGoblin.type = MONSTER_TYPE_GOBLIN;
    newGoblin.dist = 0.0f;
    newGoblin.isWeapon = false;
    newGoblin.health = 100;
    newGoblin.isAlive = true;

    sprites.push_back(newGoblin);
}

void removeDeadGoblins() {
    sprites.erase(std::remove_if(sprites.begin(), sprites.end(),
        [](const Sprite& s) {
            return !s.isAlive;
        }),
        sprites.end());
}

bool hitGoblin(float hitDamage) {
    if (sprites.empty()) return false;

    Sprite& goblin = sprites[0];
    if (!goblin.isAlive) return false;

    goblin.health -= (int)hitDamage;

    if (goblin.health <= 0) {
        goblin.isAlive = false;
        std::cout << "GOBLIN ZABITY! Usuwam go z wektora...\n";
        return true;
    }
    std::cout << "Goblin trafiony! Pozostale HP: " << goblin.health << std::endl;
    return true;
}

void moveGoblin(float playerX, float playerY) {
    if (sprites.empty() || !sprites[0].isAlive) return;

    Sprite& goblin = sprites[0];

    float dx = playerX - goblin.x;
    float dy = playerY - goblin.y;

    float distSq = dx * dx + dy * dy;
    goblin.dist = distSq;

    if (distSq > COLLISION_RADIUS * COLLISION_RADIUS) {
        float dist = std::sqrt(distSq);
        float dirX = dx / dist;
        float dirY = dy / dist;

        goblin.x += dirX * GOBLIN_CHASE_SPEED;
        goblin.y += dirY * GOBLIN_CHASE_SPEED;
    }
}

bool checkCollision(float playerX, float playerY) {
    if (sprites.empty() || !sprites[0].isAlive) return false;

    const float COLLISION_RADIUS_SQ = COLLISION_RADIUS * COLLISION_RADIUS;

    float dx = playerX - sprites[0].x;
    float dy = playerY - sprites[0].y;
    float distSq = dx * dx + dy * dy;

    return distSq < COLLISION_RADIUS_SQ;
}

void updateSprites(float playerX, float playerY) {
    if (!sprites.empty()) {
        moveGoblin(playerX, playerY);
        removeDeadGoblins();
    }
}