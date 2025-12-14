#include "Goblin.h" 
#include "Map.h" 
#include "Audio.h"
#include <vector>
#include <cmath> 
#include <iostream>
#include <algorithm> 

std::vector<Sprite> sprites;
std::vector<Fireball> fireballs;
std::vector<BloodParticle> bloodParticles;

void initMonsters() {
    sprites.clear();
    fireballs.clear();
    bloodParticles.clear();

    //Sprite goblin; goblin.x = 13.5f; goblin.y = 13.5f; goblin.type = MONSTER_TYPE_GOBLIN; goblin.health = 100; sprites.push_back(goblin);
    //Sprite flying; flying.x = 12.5f; flying.y = 12.5f; flying.type = MONSTER_TYPE_FLYING; flying.health = 150; sprites.push_back(flying);
    //Sprite walker; walker.x = 10.5f; walker.y = 10.5f; walker.type = MONSTER_TYPE_WALKING; walker.health = 200; sprites.push_back(walker);

    std::cout << "Potwory zainicjowane.\n";
}

void removeDeadMonsters() {
    sprites.erase(std::remove_if(sprites.begin(), sprites.end(), [](const Sprite& s) { return !s.isAlive; }), sprites.end());
}

bool hitMonster(int index, float hitDamage) {
    if (index < 0 || index >= sprites.size()) return false;
    Sprite& m = sprites[index];
    if (!m.isAlive) return false;

    if (m.type == MONSTER_TYPE_GOBLIN) {
        playGoblinPain();
    }

    int drops = 5;
    if (bloodParticles.size() > 200) {
        bloodParticles.erase(bloodParticles.begin(), bloodParticles.begin() + drops);
    }
    for (int i = 0; i < drops; i++) {
        BloodParticle bp;
        bp.x = m.x;
        bp.y = m.y;
        bp.z = 0.0f;
        bp.life = 1.0f;

        bp.velX = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
        bp.velY = ((float)rand() / RAND_MAX - 0.5f) * 0.1f;
        bp.velZ = ((float)rand() / RAND_MAX) * 0.1f + 0.05f;

        bloodParticles.push_back(bp);
    }

    m.health -= (int)hitDamage;
    m.state = STATE_PAIN; m.stateTimer = 0.4f;
    if (m.health <= 0) { m.isAlive = false; m.state = STATE_PAIN; return true; }
    return true;
}

void applyDamage(int& health, int& armor, int damage, float& damageAlpha) {
    if (armor > 0) {
        if (armor >= damage) { armor -= damage; damage = 0; }
        else { damage -= armor; armor = 0; }
    }
    health -= damage;

    damageAlpha = 1.0f;
}

void moveMonsters(float playerX, float playerY, float deltaTime, int& playerHealth, int& playerArmor, float& damageAlpha) {
    for (auto& m : sprites) {
        if (!m.isAlive || m.isWeapon) continue;
        float dx = playerX - m.x; float dy = playerY - m.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        m.dist = dist * dist;

        if (m.state == STATE_PAIN) {
            m.stateTimer -= deltaTime;
            if (m.stateTimer <= 0.0f) m.state = STATE_IDLE;
            continue;
        }

        float monsterRadius = 0.3f;

        if (m.type == MONSTER_TYPE_WALKING) {
            if (dist < 0.6f) {
                m.state = STATE_ATTACK;
                m.animTimer += deltaTime;
                if (m.animTimer > 0.2f) { m.fightFrame = (m.fightFrame + 1) % 2; m.animTimer = 0.0f; }
                m.attackCooldown -= deltaTime;
                if (m.attackCooldown <= 0.0f) {
                    applyDamage(playerHealth, playerArmor, 10, damageAlpha);
                    m.attackCooldown = 1.0f;
                    std::cout << "Walker uderza!\n";
                }
            }
            else {
                m.state = STATE_IDLE;
                float moveX = (dx / dist) * WALKING_MONSTER_SPEED;
                float moveY = (dy / dist) * WALKING_MONSTER_SPEED;

                if (worldMap[(int)m.y][(int)(m.x + moveX + (moveX > 0 ? monsterRadius : -monsterRadius))] == 0) m.x += moveX;
                if (worldMap[(int)(m.y + moveY + (moveY > 0 ? monsterRadius : -monsterRadius))][(int)m.x] == 0) m.y += moveY;

                m.animTimer += deltaTime;
                if (m.animTimer > 0.15f) { m.walkStep = (m.walkStep + 1) % 4; m.animTimer = 0.0f; }
            }
        }
        else if (m.type == MONSTER_TYPE_FLYING) {
            float moveX = 0.0f, moveY = 0.0f;
            if (dist > 1.0f) {
                moveX = (dx / dist) * FLYING_MONSTER_SPEED;
                moveY = (dy / dist) * FLYING_MONSTER_SPEED;
            }

            if (worldMap[(int)m.y][(int)(m.x + moveX + (moveX > 0 ? monsterRadius : -monsterRadius))] == 0) m.x += moveX;
            if (worldMap[(int)(m.y + moveY + (moveY > 0 ? monsterRadius : -monsterRadius))][(int)m.x] == 0) m.y += moveY;

            m.attackCooldown -= deltaTime;
            if (m.attackCooldown <= 0.0f && dist < 8.0f) {
                m.state = STATE_ATTACK; m.stateTimer = 0.3f; m.attackCooldown = 2.0f;
                Fireball fb; fb.x = m.x; fb.y = m.y; fb.dirX = (dx / dist) * 0.1f; fb.dirY = (dy / dist) * 0.1f; fb.active = true;
                fireballs.push_back(fb);
            }
            if (m.state == STATE_ATTACK && m.stateTimer > 0) m.stateTimer -= deltaTime;
            else if (m.state != STATE_PAIN) m.state = STATE_IDLE;
        }
        else if (m.type == MONSTER_TYPE_GOBLIN) {

            m.soundTimer -= deltaTime;
            if (m.soundTimer <= 0.0f) {
                if (dist < 15.0f) {
                    playGoblinIdle();
                }
                m.soundTimer = 3.0f + ((float)rand() / RAND_MAX) * 4.0f;
            }

            if (dist < 0.7f) {
                m.attackCooldown -= deltaTime;
                if (m.attackCooldown <= 0.0f) {
                    playGoblinAttack();
                    m.attackCooldown = 1.0f;
                }
            }

            float moveX = 0.0f, moveY = 0.0f;
            if (dist > COLLISION_RADIUS) {
                moveX = (dx / dist) * GOBLIN_CHASE_SPEED;
                moveY = (dy / dist) * GOBLIN_CHASE_SPEED;
            }
            if (worldMap[(int)m.y][(int)(m.x + moveX + (moveX > 0 ? monsterRadius : -monsterRadius))] == 0) m.x += moveX;
            if (worldMap[(int)(m.y + moveY + (moveY > 0 ? monsterRadius : -monsterRadius))][(int)m.x] == 0) m.y += moveY;
        }
    }
}

void updateBloodParticles(float dt) {
    for (auto& bp : bloodParticles) {
        if (bp.z > -0.5f) {
            bp.x += bp.velX;
            bp.y += bp.velY;
            bp.z += bp.velZ;

            bp.velZ -= 1.5f * dt;

        }
        else {
            bp.z = -0.5f;
            bp.velX = 0;
            bp.velY = 0;
            bp.velZ = 0;
        }
    }

    bloodParticles.erase(std::remove_if(bloodParticles.begin(), bloodParticles.end(),
        [](const BloodParticle& p) { return p.life <= 0.0f; }), bloodParticles.end());
}

void updateFireballs(float playerX, float playerY, float deltaTime, int& playerHealth, int& playerArmor, float& damageAlpha) {
    for (auto& fb : fireballs) {
        if (!fb.active) continue;
        fb.x += fb.dirX; fb.y += fb.dirY;
        int tile = worldMap[(int)fb.y][(int)fb.x];
        if (tile == 1 || tile == 2) { fb.active = false; continue; }
        float dx = fb.x - playerX; float dy = fb.y - playerY;
        if (dx * dx + dy * dy < 0.2f) {
            applyDamage(playerHealth, playerArmor, 15, damageAlpha);
            fb.active = false;
            std::cout << "Kula ognia trafia!\n";
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