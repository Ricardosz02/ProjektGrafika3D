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
std::vector<BloodParticle> fireParticles;

const int TYPE_BARREL = 50;
const int TYPE_EXPLOSION = 90;
const int TYPE_FIRE = 91;

const float BARREL_RADIUS = 8.0f;
const int BARREL_DAMAGE = 150;

extern int (*worldMap)[MAP_WIDTH];
extern int activeMapIndex;

void initMonsters() {
    sprites.clear();
    fireballs.clear();
    bloodParticles.clear();
    fireParticles.clear();

    //Sprite goblin; goblin.x = 13.5f; goblin.y = 13.5f; goblin.type = MONSTER_TYPE_GOBLIN; goblin.health = 100; sprites.push_back(goblin);
    //Sprite flying; flying.x = 12.5f; flying.y = 12.5f; flying.type = MONSTER_TYPE_FLYING; flying.health = 200; sprites.push_back(flying);
    //Sprite walker; walker.x = 10.5f; walker.y = 10.5f; walker.type = MONSTER_TYPE_WALKING; walker.health = 140; sprites.push_back(walker);
    /*
    Sprite barrel;
    barrel.x = 12.0f;
    barrel.y = 12.0f;
    barrel.type = TYPE_BARREL;
    barrel.health = 1;
    barrel.isAlive = true;
    sprites.push_back(barrel);
    */

    if (activeMapIndex == 1) {
        Sprite w1;
        w1.x = 3.0f;
        w1.y = 42.0f;
        w1.type = MONSTER_TYPE_WALKING;
        w1.health = 140;
        w1.isAlive = true;
        sprites.push_back(w1);

        Sprite w2;
        w2.x = 11.0f;
        w2.y = 42.0f;
        w2.type = MONSTER_TYPE_WALKING;
        w2.health = 140;
        w2.isAlive = true;
        sprites.push_back(w2);

        Sprite w3;
        w3.x = 29.0f;
        w3.y = 54.0f;
        w3.type = MONSTER_TYPE_WALKING;
        w3.health = 140;
        w3.isAlive = true;
        sprites.push_back(w3);
 
        Sprite w4;
        w4.x = 37.0f;
        w4.y = 53.0f;
        w4.type = MONSTER_TYPE_WALKING;
        w4.health = 140;
        w4.isAlive = true;
        sprites.push_back(w4);

        std::cout << "Potwory zainicjowane.\n";
    }

    if (activeMapIndex == 2) {
        Sprite w1;
        w1.x = 9.5f;
        w1.y = 6.0f;
        w1.type = MONSTER_TYPE_WALKING;
        w1.health = 140;
        w1.isAlive = true;
        sprites.push_back(w1);

        Sprite w2;
        w2.x = 9.5f;
        w2.y = 9.0f;
        w2.type = MONSTER_TYPE_WALKING;
        w2.health = 140;
        w2.isAlive = true;
        sprites.push_back(w2);

        Sprite w3;
        w3.x = 41.0f;
        w3.y = 23.5f;
        w3.type = MONSTER_TYPE_WALKING;
        w3.health = 140;
        w3.isAlive = true;
        sprites.push_back(w3);


        Sprite w4;
        w4.x = 26.5f;
        w4.y = 7.5f;
        w4.type = MONSTER_TYPE_FLYING;
        w4.health = 200;
        w4.isAlive = true;
        sprites.push_back(w4);


        Sprite w5;
        w5.x = 25.0f;
        w5.y = 25.0f;
        w5.type = MONSTER_TYPE_FLYING;
        w5.health = 200;
        w5.isAlive = true;
        sprites.push_back(w5);


    }
}

void removeDeadMonsters() {
    sprites.erase(std::remove_if(sprites.begin(), sprites.end(), [](const Sprite& s) { return !s.isAlive; }), sprites.end());
}

void updateFireParticles(float dt) {
    for (auto& fp : fireParticles) {
        fp.x += fp.velX;
        fp.y += fp.velY;
        fp.z += fp.velZ;

        fp.velZ -= 0.5f * dt;
        fp.life -= 1.5f * dt;

        if (fp.z < -0.5f || fp.z > 0.5f) {
            fp.life = -1.0f;
            continue;
        }

        int mapX = (int)fp.x;
        int mapY = (int)fp.y;

        if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT) {
            if (worldMap[mapY][mapX] > 0) {
                fp.life = -1.0f;
            }
        }
    }

    fireParticles.erase(std::remove_if(fireParticles.begin(), fireParticles.end(),
        [](const BloodParticle& p) { return p.life <= 0.0f; }), fireParticles.end());
}

bool hitMonster(int index, float hitDamage) {
    if (index < 0 || index >= sprites.size()) return false;
    Sprite& m = sprites[index];
    if (!m.isAlive) return false;
    if (m.type == TYPE_EXPLOSION) return false;

    if (m.type == TYPE_BARREL) {
        m.health -= (int)hitDamage;
        if (m.health <= 0) {
            m.type = TYPE_EXPLOSION;
            m.stateTimer = 0.5f;
            m.isAlive = true;

            playExplosionSound();

            for (int i = 0; i < 500; i++) {
                BloodParticle fp;
                fp.x = m.x; fp.y = m.y; fp.z = 0.0f; fp.life = 1.0f;
                fp.velX = ((float)rand() / RAND_MAX - 0.5f) * 0.4f;
                fp.velY = ((float)rand() / RAND_MAX - 0.5f) * 0.4f;
                fp.velZ = ((float)rand() / RAND_MAX) * 0.5f;
                fireParticles.push_back(fp);
            }

            for (auto& other : sprites) {
                if (&other == &m || !other.isAlive || other.type == TYPE_BARREL || other.type == TYPE_EXPLOSION) continue;
                float dx = m.x - other.x;
                float dy = m.y - other.y;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist < 3.0f) {
                    other.health -= 150;
                    other.state = STATE_PAIN;
                    if (other.health <= 0) other.isAlive = false;
                }
            }
        }
        return true;
    }

    if (m.type == MONSTER_TYPE_GOBLIN) {
        playGoblinPain();
    }
    else if (m.type == MONSTER_TYPE_FLYING) {
        playFlyingPain();
    }
    else if (m.type == MONSTER_TYPE_WALKING) {
        playWalkerPain();
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
    if (damage > 0) {
        health -= damage;
        playPlayerPain();
    }

    damageAlpha = 1.0f;
}

bool checkLineOfSight(float mx, float my, float px, float py) {
    float dx = px - mx;
    float dy = py - my;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < 0.5f) return true;

    float steps = dist / 0.5f;
    float stepX = dx / steps;
    float stepY = dy / steps;

    float currentX = mx;
    float currentY = my;

    for (int i = 0; i < (int)steps; i++) {
        currentX += stepX;
        currentY += stepY;

        if (currentX < 0 || currentX >= MAP_WIDTH || currentY < 0 || currentY >= MAP_HEIGHT) return false;

        int mapX = (int)currentX;
        int mapY = (int)currentY;
        int tile = worldMap[mapY][mapX];

        if (tile == 1) return false;

        if ((tile >= 2 && tile <= 5) || tile == 8) {
            for (const auto& d : doors) {
                if (d.x == mapX && d.y == mapY) {
                    if (d.openAmount < 0.7f) return false;
                    break;
                }
            }
        }
    }

    return true;
}

void moveMonsters(float playerX, float playerY, float deltaTime, int& playerHealth, int& playerArmor, float& damageAlpha) {
    updateFireParticles(deltaTime);

    for (auto& m : sprites) {
        if (!m.isAlive || m.isWeapon) continue;

        if (m.type == TYPE_EXPLOSION) {
            if (m.stateTimer > 0.45f) {
                float dx = m.x - playerX; float dy = m.y - playerY;
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist < 3.0f) {
                    applyDamage(playerHealth, playerArmor, 30, damageAlpha);
                }
            }
            m.stateTimer -= deltaTime;
            if (m.stateTimer <= 0.0f) m.isAlive = false;
            continue;
        }
        if (m.type == TYPE_BARREL) continue;

        for (const auto& other : sprites) {
            if (&m == &other || !other.isAlive || other.isWeapon) continue;
            if (other.type == TYPE_BARREL || other.type == TYPE_EXPLOSION) continue;

            float dx = m.x - other.x;
            float dy = m.y - other.y;
            float dist = std::sqrt(dx * dx + dy * dy);

            float minSpacing = 0.6f;

            if (dist < minSpacing && dist > 0.001f) {
                float pushX = dx / dist;
                float pushY = dy / dist;

                float pushStrength = (minSpacing - dist) * 2.0f * deltaTime;

                float newX = m.x + pushX * pushStrength;
                float newY = m.y + pushY * pushStrength;

                if (worldMap[(int)m.y][(int)newX] == 0) m.x = newX;
                if (worldMap[(int)newY][(int)m.x] == 0) m.y = newY;
            }
        }

        float dx = playerX - m.x; float dy = playerY - m.y;
        float dist = std::sqrt(dx * dx + dy * dy);
        m.dist = dist * dist;

        bool canSee = checkLineOfSight(m.x, m.y, playerX, playerY);

        if (m.state == STATE_PAIN) {
            m.stateTimer -= deltaTime;
            if (m.stateTimer <= 0.0f) m.state = STATE_IDLE;
            continue;
        }

        if (!canSee && dist > 1.5f) {
            m.state = STATE_IDLE;
            continue;
        }

        float monsterRadius = 0.3f;

        if (m.type == MONSTER_TYPE_WALKING) {

            m.soundTimer -= deltaTime;
            if (m.soundTimer <= 0.0f) {
                if (dist < 10.0f) {
                    playWalkerIdle();
                }
                m.soundTimer = 2.0f + ((float)rand() / RAND_MAX) * 2.0f;
            }

            if (dist < 0.6f) {
                m.state = STATE_ATTACK;
                m.animTimer += deltaTime;
                if (m.animTimer > 0.3f) { m.fightFrame = (m.fightFrame + 1) % 2; m.animTimer = 0.0f; }
                m.attackCooldown -= deltaTime;
                if (m.attackCooldown <= 0.0f) {

                    playWalkerAttack();

                    applyDamage(playerHealth, playerArmor, 15, damageAlpha); // Obrażenia Walker
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
            m.soundTimer -= deltaTime;
            if (m.soundTimer <= 0.0f) {
                if (dist < 15.0f && (rand() % 100 < 25)) {
                    playFlyingIdle();
                }
                m.soundTimer = 2.0f + ((float)rand() / RAND_MAX) * 3.0f;
            }

            float moveX = 0.0f, moveY = 0.0f;
            if (dist > 1.0f) {
                moveX = (dx / dist) * FLYING_MONSTER_SPEED;
                moveY = (dy / dist) * FLYING_MONSTER_SPEED;
            }

            if (worldMap[(int)m.y][(int)(m.x + moveX + (moveX > 0 ? monsterRadius : -monsterRadius))] == 0) m.x += moveX;
            if (worldMap[(int)(m.y + moveY + (moveY > 0 ? monsterRadius : -monsterRadius))][(int)m.x] == 0) m.y += moveY;

            m.attackCooldown -= deltaTime;
            if (m.attackCooldown <= 0.0f && dist < 8.0f) {
                playFlyingAttack();
                m.state = STATE_ATTACK;
                m.stateTimer = 0.3f;
                m.attackCooldown = 3.5f + ((float)rand() / RAND_MAX) * 2.0f;

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
            applyDamage(playerHealth, playerArmor, 20, damageAlpha); // Obrażenia Flying
            fb.active = false;
            std::cout << "Kula ognia trafia!\n";
        }
    }
    fireballs.erase(std::remove_if(fireballs.begin(), fireballs.end(), [](const Fireball& f) { return !f.active; }), fireballs.end());
}

bool checkCollision(float playerX, float playerY) {
    for (const auto& m : sprites) {
        if (!m.isAlive || m.isWeapon) continue;

        if (m.type == TYPE_BARREL || m.type == TYPE_EXPLOSION) continue;

        if (m.type == MONSTER_TYPE_GOBLIN) {
            float dx = playerX - m.x; float dy = playerY - m.y;
            if (dx * dx + dy * dy < COLLISION_RADIUS * COLLISION_RADIUS) return true;
        }
    }
    return false;
}