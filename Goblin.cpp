#include "Goblin.h" 
#include "Map.h" 
#include <vector>
#include <cmath> 
#include <iostream>
#include <algorithm> 

std::vector<Sprite> sprites;
std::vector<Fireball> fireballs;

void initMonsters() {
    sprites.clear();
    fireballs.clear();

    // 1. Zwykły Goblin
    //Sprite goblin;
    //goblin.x = 13.5f; goblin.y = 13.5f;
    //goblin.type = MONSTER_TYPE_GOBLIN;
    //goblin.health = 100;
    //sprites.push_back(goblin);

    // 2. Latający
    Sprite flying;
    flying.x = 12.5f; flying.y = 12.5f;
    flying.type = MONSTER_TYPE_FLYING;
    flying.health = 150;
    sprites.push_back(flying);

    // 3. Walker
    //Sprite walker;
    //walker.x = 10.5f; walker.y = 10.5f;
    //walker.type = MONSTER_TYPE_WALKING;
    //walker.health = 200;
    //sprites.push_back(walker);

    std::cout << "Potwory zainicjowane.\n";
}

void removeDeadMonsters() {
    sprites.erase(std::remove_if(sprites.begin(), sprites.end(),
        [](const Sprite& s) { return !s.isAlive; }), sprites.end());
}

bool hitMonster(int index, float hitDamage) {
    if (index < 0 || index >= sprites.size()) return false;
    Sprite& m = sprites[index];
    if (!m.isAlive) return false;

    m.health -= (int)hitDamage;
    m.state = STATE_PAIN;
    m.stateTimer = 0.4f;

    if (m.health <= 0) {
        m.isAlive = false;
        m.state = STATE_PAIN;
        return true;
    }
    return true;
}

// Funkcja pomocnicza do zadawania obrażeń z uwzględnieniem pancerza
void applyDamage(int& health, int& armor, int damage) {
    if (armor > 0) {
        if (armor >= damage) {
            armor -= damage;
            damage = 0; // Pancerz wchłonął wszystko
        }
        else {
            damage -= armor; // Pancerz zniszczony, reszta idzie w HP
            armor = 0;
        }
    }
    health -= damage;
}

void moveMonsters(float playerX, float playerY, float deltaTime, int& playerHealth, int& playerArmor) {
    for (auto& m : sprites) {
        if (!m.isAlive || m.isWeapon) continue;

        float dx = playerX - m.x;
        float dy = playerY - m.y;
        float distSq = dx * dx + dy * dy;
        float dist = std::sqrt(distSq);
        m.dist = distSq;

        if (m.state == STATE_PAIN) {
            m.stateTimer -= deltaTime;
            if (m.stateTimer <= 0.0f) m.state = STATE_IDLE;
            continue;
        }

        // --- WALKER ---
        if (m.type == MONSTER_TYPE_WALKING) {
            // ZMIANA TUTAJ: Zmniejszamy dystans ataku z 1.5f na 0.8f
            // Dzięki temu potwór musi podejść bliżej, żeby uderzyć
            if (dist < 0.5f) {
                m.state = STATE_ATTACK;
                m.animTimer += deltaTime;
                if (m.animTimer > 0.2f) { m.fightFrame = (m.fightFrame + 1) % 2; m.animTimer = 0.0f; }

                m.attackCooldown -= deltaTime;
                if (m.attackCooldown <= 0.0f) {
                    applyDamage(playerHealth, playerArmor, 10);
                    m.attackCooldown = 1.0f;
                    std::cout << "Walker uderza! HP: " << playerHealth << " Armor: " << playerArmor << "\n";
                }
            }
            else {
                m.state = STATE_IDLE;
                float moveX = (dx / dist) * WALKING_MONSTER_SPEED;
                float moveY = (dy / dist) * WALKING_MONSTER_SPEED;

                // Kolizja ze ścianami (trochę mniejszy margines 0.2f bo potwór mniejszy)
                if (worldMap[(int)(m.y)][(int)(m.x + moveX + 0.2f)] == 0) m.x += moveX;
                if (worldMap[(int)(m.y + moveY + 0.2f)][(int)(m.x)] == 0) m.y += moveY;

                m.animTimer += deltaTime;
                if (m.animTimer > 0.15f) { m.walkStep = (m.walkStep + 1) % 4; m.animTimer = 0.0f; }
            }
        }
        // --- FLYING ---
        else if (m.type == MONSTER_TYPE_FLYING) {
            if (dist > 1.0f) { m.x += (dx / dist) * FLYING_MONSTER_SPEED; m.y += (dy / dist) * FLYING_MONSTER_SPEED; }
            m.attackCooldown -= deltaTime;
            if (m.attackCooldown <= 0.0f && dist < 8.0f) {
                m.state = STATE_ATTACK; m.stateTimer = 0.3f; m.attackCooldown = 2.0f;
                Fireball fb; fb.x = m.x; fb.y = m.y; fb.dirX = (dx / dist) * 0.1f; fb.dirY = (dy / dist) * 0.1f; fb.active = true;
                fireballs.push_back(fb);
            }
            if (m.state == STATE_ATTACK && m.stateTimer > 0) m.stateTimer -= deltaTime;
            else if (m.state != STATE_PAIN) m.state = STATE_IDLE;
        }
        // --- GOBLIN ---
        else if (m.type == MONSTER_TYPE_GOBLIN) {
            if (dist > COLLISION_RADIUS) { m.x += (dx / dist) * GOBLIN_CHASE_SPEED; m.y += (dy / dist) * GOBLIN_CHASE_SPEED; }
        }
    }
}

void updateFireballs(float playerX, float playerY, float deltaTime, int& playerHealth, int& playerArmor) {
    for (auto& fb : fireballs) {
        if (!fb.active) continue;
        fb.x += fb.dirX; fb.y += fb.dirY;
        if (worldMap[(int)fb.y][(int)fb.x] > 0) { fb.active = false; continue; }
        float dx = fb.x - playerX; float dy = fb.y - playerY;
        if (dx * dx + dy * dy < 0.2f) {
            applyDamage(playerHealth, playerArmor, 15);
            fb.active = false;
            std::cout << "Kula ognia! HP: " << playerHealth << " Armor: " << playerArmor << "\n";
        }
    }
    fireballs.erase(std::remove_if(fireballs.begin(), fireballs.end(), [](const Fireball& f) { return !f.active; }), fireballs.end());
}

bool checkCollision(float playerX, float playerY) {
    for (const auto& m : sprites) {
        if (!m.isAlive || m.isWeapon) continue;
        if (m.type == MONSTER_TYPE_GOBLIN) {
            float dx = playerX - m.x; float dy = playerY - m.y;
            if (dx * dx + dy * dy < COLLISION_RADIUS * COLLISION_RADIUS) return true;
        }
    }
    return false;
}