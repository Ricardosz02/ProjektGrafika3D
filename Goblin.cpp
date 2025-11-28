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
    Sprite goblin;
    goblin.x = 10.5f; goblin.y = 10.5f;
    goblin.type = MONSTER_TYPE_GOBLIN;
    goblin.health = 100;
    sprites.push_back(goblin);

    // 2. Latający
    Sprite flying;
    flying.x = 12.5f; flying.y = 12.5f;
    flying.type = MONSTER_TYPE_FLYING;
    flying.health = 150;
    sprites.push_back(flying);
    
    // 3. WALKING MONSTER
    Sprite walker;
    walker.x = 8.5f; walker.y = 8.5f;
    walker.type = MONSTER_TYPE_WALKING;
    walker.health = 200;
    sprites.push_back(walker);
}

void removeDeadMonsters() {
    // Tutaj prosta wersja: usuwamy całkowicie.
    sprites.erase(std::remove_if(sprites.begin(), sprites.end(),
        [](const Sprite& s) { return !s.isAlive; }), sprites.end());
}

bool hitMonster(int index, float hitDamage) {
    if (index < 0 || index >= sprites.size()) return false;
    Sprite& m = sprites[index];
    if (!m.isAlive) return false;

    m.health -= (int)hitDamage;

    // Reakcja na ból
    m.state = STATE_PAIN;
    m.stateTimer = 0.4f; // Czas wyświetlania klatki "hit"

    if (m.health <= 0) {
        m.isAlive = false;
        m.state = STATE_PAIN; // Żeby wyświetlić hit_5 jako śmierć
        std::cout << "Potwor zabity!\n";
        return true;
    }
    return true;
}

void moveMonsters(float playerX, float playerY, float deltaTime, int& playerHealth) {
    for (auto& m : sprites) {
        if (!m.isAlive || m.isWeapon) continue;

        float dx = playerX - m.x;
        float dy = playerY - m.y;
        float distSq = dx * dx + dy * dy;
        float dist = std::sqrt(distSq);
        m.dist = distSq;

        // Licznik stanów (ból mija z czasem)
        if (m.state == STATE_PAIN) {
            m.stateTimer -= deltaTime;
            if (m.stateTimer <= 0.0f) m.state = STATE_IDLE;
            continue; // Jak go boli, to się nie rusza
        }

        // --- TYP 3: WALKING MONSTER ---
        if (m.type == MONSTER_TYPE_WALKING) {

            // Atak wręcz (gdy jest blisko)
            if (dist < 1.0f) {
                m.state = STATE_ATTACK;

                // Animacja walki
                m.animTimer += deltaTime;
                if (m.animTimer > 0.2f) { // Zmiana klatki ataku co 0.2s
                    m.fightFrame = (m.fightFrame + 1) % 2; // 0, 1, 0, 1...
                    m.animTimer = 0.0f;
                }

                // Zadawanie obrażeń
                m.attackCooldown -= deltaTime;
                if (m.attackCooldown <= 0.0f) {
                    playerHealth -= 10;
                    m.attackCooldown = 1.0f; // Bije co 1 sekunde
                    std::cout << "Walker uderza! HP gracza: " << playerHealth << "\n";
                }
            }
            // Chodzenie (gdy daleko)
            else {
                m.state = STATE_IDLE;

                // Ruch
                float moveX = (dx / dist) * WALKING_MONSTER_SPEED;
                float moveY = (dy / dist) * WALKING_MONSTER_SPEED;

                // Prosta kolizja ze ścianami
                if (worldMap[(int)(m.y)][(int)(m.x + moveX + 0.3f)] == 0) m.x += moveX;
                if (worldMap[(int)(m.y + moveY + 0.3f)][(int)(m.x)] == 0) m.y += moveY;

                // Animacja Chodu: 1 -> 3 -> 1 -> 2
                m.animTimer += deltaTime;
                if (m.animTimer > 0.15f) { // Zmiana kroku co 0.15s
                    m.walkStep = (m.walkStep + 1) % 4; // 0, 1, 2, 3 -> 0...
                    m.animTimer = 0.0f;
                }
            }
        }
        // --- TYP 2: FLYING ---
        else if (m.type == MONSTER_TYPE_FLYING) {
            if (dist > 1.0f) {
                m.x += (dx / dist) * FLYING_MONSTER_SPEED;
                m.y += (dy / dist) * FLYING_MONSTER_SPEED;
            }
            m.attackCooldown -= deltaTime;
            if (m.attackCooldown <= 0.0f && dist < 8.0f) {
                m.state = STATE_ATTACK;
                m.stateTimer = 0.3f;
                m.attackCooldown = 2.0f;
                Fireball fb; fb.x = m.x; fb.y = m.y;
                fb.dirX = (dx / dist) * 0.1f; fb.dirY = (dy / dist) * 0.1f; fb.active = true;
                fireballs.push_back(fb);
            }
            if (m.state == STATE_ATTACK && m.stateTimer > 0) m.stateTimer -= deltaTime;
            else if (m.state != STATE_PAIN) m.state = STATE_IDLE;
        }
        // --- TYP 1: GOBLIN ---
        else if (m.type == MONSTER_TYPE_GOBLIN) {
            if (dist > COLLISION_RADIUS) {
                m.x += (dx / dist) * GOBLIN_CHASE_SPEED;
                m.y += (dy / dist) * GOBLIN_CHASE_SPEED;
            }
        }
    }
}

void updateFireballs(float playerX, float playerY, float deltaTime, int& playerHealth) {
    for (auto& fb : fireballs) {
        if (!fb.active) continue;
        fb.x += fb.dirX; fb.y += fb.dirY;
        if (worldMap[(int)fb.y][(int)fb.x] > 0) { fb.active = false; continue; }
        float dx = fb.x - playerX; float dy = fb.y - playerY;
        if (dx * dx + dy * dy < 0.2f) {
            playerHealth -= 15; fb.active = false;
            std::cout << "Oberwales kula ognia! HP: " << playerHealth << std::endl;
        }
    }
    fireballs.erase(std::remove_if(fireballs.begin(), fireballs.end(),
        [](const Fireball& f) { return !f.active; }), fireballs.end());
}

bool checkCollision(float playerX, float playerY) {
    // Kolizja tylko z Goblinem (typ 1), Walker bije z dystansu melee
    for (const auto& m : sprites) {
        if (!m.isAlive || m.isWeapon) continue;
        if (m.type == MONSTER_TYPE_GOBLIN) {
            float dx = playerX - m.x; float dy = playerY - m.y;
            if (dx * dx + dy * dy < COLLISION_RADIUS * COLLISION_RADIUS) return true;
        }
    }
    return false;
}

void updateSprites(float playerX, float playerY) {
    float deltaTime = 0.016f;
    // UWAGA: Funkcja updateSprites w main.cpp musi teraz przyjmować playerHealth!
    // W tej wersji pliku zakladam, ze przekazujemy health w moveMonsters.
}