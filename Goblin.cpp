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

extern bool isGameWon;

void spawnWave(int waveNumber) {
    std::cout << "--- BOSS FIGHT: FALA " << waveNumber << " ---" << std::endl;

    if (waveNumber == 1) {
        Sprite s1; s1.x = 47.0f; s1.y = 23.0f; s1.type = MONSTER_TYPE_WALKING; s1.health = 140; s1.isAlive = true; sprites.push_back(s1);
        Sprite s2; s2.x = 47.0f; s2.y = 25.0f; s2.type = MONSTER_TYPE_WALKING; s2.health = 140; s2.isAlive = true; sprites.push_back(s2);
        Sprite s3; s3.x = 47.0f; s3.y = 27.0f; s3.type = MONSTER_TYPE_WALKING; s3.health = 140; s3.isAlive = true; sprites.push_back(s3);
        Sprite s4; s4.x = 47.0f; s4.y = 29.0f; s4.type = MONSTER_TYPE_WALKING; s4.health = 140; s4.isAlive = true; sprites.push_back(s4);
        Sprite s5; s5.x = 47.0f; s5.y = 31.0f; s5.type = MONSTER_TYPE_WALKING; s5.health = 140; s5.isAlive = true; sprites.push_back(s5);
        Sprite s6; s6.x = 47.0f; s6.y = 33.0f; s6.type = MONSTER_TYPE_WALKING; s6.health = 140; s6.isAlive = true; sprites.push_back(s6);
        Sprite s7; s7.x = 47.0f; s7.y = 35.0f; s7.type = MONSTER_TYPE_WALKING; s7.health = 140; s7.isAlive = true; sprites.push_back(s7);
        Sprite s8; s8.x = 47.0f; s8.y = 37.0f; s8.type = MONSTER_TYPE_WALKING; s8.health = 140; s8.isAlive = true; sprites.push_back(s8);
    }
    else if (waveNumber == 2) {
        Sprite s1; s1.x = 47.0f; s1.y = 23.0f; s1.type = MONSTER_TYPE_FLYING; s1.health = 200; s1.isAlive = true; sprites.push_back(s1);
        Sprite s2; s2.x = 47.0f; s2.y = 26.0f; s2.type = MONSTER_TYPE_FLYING; s2.health = 200; s2.isAlive = true; sprites.push_back(s2);
        Sprite s3; s3.x = 47.0f; s3.y = 29.0f; s3.type = MONSTER_TYPE_FLYING; s3.health = 200; s3.isAlive = true; sprites.push_back(s3);
        Sprite s4; s4.x = 47.0f; s4.y = 31.0f; s4.type = MONSTER_TYPE_FLYING; s4.health = 200; s4.isAlive = true; sprites.push_back(s4);
        Sprite s5; s5.x = 47.0f; s5.y = 34.0f; s5.type = MONSTER_TYPE_FLYING; s5.health = 200; s5.isAlive = true; sprites.push_back(s5);
        Sprite s6; s6.x = 47.0f; s6.y = 37.0f; s6.type = MONSTER_TYPE_FLYING; s6.health = 200; s6.isAlive = true; sprites.push_back(s6);
    }
    else if (waveNumber == 3) {
        Sprite s1; s1.x = 47.0f; s1.y = 21.0f; s1.type = MONSTER_TYPE_GOBLIN; s1.health = 100; s1.isAlive = true; sprites.push_back(s1);
        //Sprite s2; s2.x = 47.0f; s2.y = 23.0f; s2.type = MONSTER_TYPE_GOBLIN; s2.health = 100; s2.isAlive = true; sprites.push_back(s2);
        Sprite s3; s3.x = 47.0f; s3.y = 25.0f; s3.type = MONSTER_TYPE_GOBLIN; s3.health = 100; s3.isAlive = true; sprites.push_back(s3);
        //Sprite s4; s4.x = 47.0f; s4.y = 27.0f; s4.type = MONSTER_TYPE_GOBLIN; s4.health = 100; s4.isAlive = true; sprites.push_back(s4);
        Sprite s5; s5.x = 47.0f; s5.y = 29.0f; s5.type = MONSTER_TYPE_GOBLIN; s5.health = 100; s5.isAlive = true; sprites.push_back(s5);
        Sprite s6; s6.x = 47.0f; s6.y = 31.0f; s6.type = MONSTER_TYPE_GOBLIN; s6.health = 100; s6.isAlive = true; sprites.push_back(s6);
        //Sprite s7; s7.x = 47.0f; s7.y = 33.0f; s7.type = MONSTER_TYPE_GOBLIN; s7.health = 100; s7.isAlive = true; sprites.push_back(s7);
        Sprite s8; s8.x = 47.0f; s8.y = 35.0f; s8.type = MONSTER_TYPE_GOBLIN; s8.health = 100; s8.isAlive = true; sprites.push_back(s8);
        //Sprite s9; s9.x = 47.0f; s9.y = 37.0f; s9.type = MONSTER_TYPE_GOBLIN; s9.health = 100; s9.isAlive = true; sprites.push_back(s9);
        Sprite s10; s10.x = 47.0f; s10.y = 39.0f; s10.type = MONSTER_TYPE_GOBLIN; s10.health = 100; s10.isAlive = true; sprites.push_back(s10);
    }
    else if (waveNumber == 4) {
        Sprite w1; w1.x = 45.0f; w1.y = 26.0f; w1.type = MONSTER_TYPE_WALKING; w1.health = 140; w1.isAlive = true; sprites.push_back(w1);
        Sprite w2; w2.x = 45.0f; w2.y = 30.0f; w2.type = MONSTER_TYPE_WALKING; w2.health = 140; w2.isAlive = true; sprites.push_back(w2);
        Sprite w3; w3.x = 45.0f; w3.y = 34.0f; w3.type = MONSTER_TYPE_WALKING; w3.health = 140; w3.isAlive = true; sprites.push_back(w3);
        //Sprite w4; w4.x = 45.0f; w4.y = 34.0f; w4.type = MONSTER_TYPE_WALKING; w4.health = 140; w4.isAlive = true; sprites.push_back(w4);

        Sprite f1; f1.x = 47.0f; f1.y = 25.0f; f1.type = MONSTER_TYPE_FLYING; f1.health = 200; f1.isAlive = true; sprites.push_back(f1);
        //Sprite f2; f2.x = 47.0f; f2.y = 29.0f; f2.type = MONSTER_TYPE_FLYING; f2.health = 200; f2.isAlive = true; sprites.push_back(f2);
        //Sprite f3; f3.x = 47.0f; f3.y = 31.0f; f3.type = MONSTER_TYPE_FLYING; f3.health = 200; f3.isAlive = true; sprites.push_back(f3);
        Sprite f4; f4.x = 47.0f; f4.y = 35.0f; f4.type = MONSTER_TYPE_FLYING; f4.health = 200; f4.isAlive = true; sprites.push_back(f4);

        Sprite g1; g1.x = 32.0f; g1.y = 39.0f; g1.type = MONSTER_TYPE_GOBLIN; g1.health = 100; g1.isAlive = true; sprites.push_back(g1);
        Sprite g2; g2.x = 32.0f; g2.y = 21.0f; g2.type = MONSTER_TYPE_GOBLIN; g2.health = 100; g2.isAlive = true; sprites.push_back(g2);
        //Sprite g3; g3.x = 34.0f; g3.y = 39.0f; g3.type = MONSTER_TYPE_GOBLIN; g3.health = 100; g3.isAlive = true; sprites.push_back(g3);
        //Sprite g4; g4.x = 34.0f; g4.y = 21.0f; g4.type = MONSTER_TYPE_GOBLIN; g4.health = 100; g4.isAlive = true; sprites.push_back(g4);
    }
}

void initMonsters() {
    sprites.clear();
    fireballs.clear();
    bloodParticles.clear();
    fireParticles.clear();

    if (activeMapIndex == 1) {
        Sprite w1; w1.x = 3.0f; w1.y = 42.0f; w1.type = MONSTER_TYPE_WALKING; w1.health = 140; w1.isAlive = true; sprites.push_back(w1);
        Sprite w2; w2.x = 11.0f; w2.y = 42.0f; w2.type = MONSTER_TYPE_WALKING; w2.health = 140; w2.isAlive = true; sprites.push_back(w2);
        Sprite w3; w3.x = 29.0f; w3.y = 54.0f; w3.type = MONSTER_TYPE_WALKING; w3.health = 140; w3.isAlive = true; sprites.push_back(w3);
        Sprite w4; w4.x = 37.0f; w4.y = 53.0f; w4.type = MONSTER_TYPE_WALKING; w4.health = 140; w4.isAlive = true; sprites.push_back(w4);
        Sprite w5; w5.x = 44.0f; w5.y = 40.0f; w5.type = MONSTER_TYPE_WALKING; w5.health = 140; w5.isAlive = true; sprites.push_back(w5);
        Sprite w6; w6.x = 23.0f; w6.y = 34.0f; w6.type = MONSTER_TYPE_WALKING; w6.health = 140; w6.isAlive = true; sprites.push_back(w6);
    }

    if (activeMapIndex == 2) {
        Sprite w1; w1.x = 9.0f; w1.y = 9.0f; w1.type = MONSTER_TYPE_WALKING; w1.health = 140; w1.isAlive = true; sprites.push_back(w1);
        Sprite w2; w2.x = 6.0f; w2.y = 9.0f; w2.type = MONSTER_TYPE_WALKING; w2.health = 140; w2.isAlive = true; sprites.push_back(w2);
        Sprite w3; w3.x = 41.0f; w3.y = 24.0f; w3.type = MONSTER_TYPE_WALKING; w3.health = 140; w3.isAlive = true; sprites.push_back(w3);
        Sprite w4; w4.x = 41.0f; w4.y = 35.0f; w4.type = MONSTER_TYPE_WALKING; w4.health = 140; w4.isAlive = true; sprites.push_back(w4);
        Sprite w5; w5.x = 41.0f; w5.y = 41.0f; w5.type = MONSTER_TYPE_WALKING; w5.health = 140; w5.isAlive = true; sprites.push_back(w5);

        Sprite w6; w6.x = 26.0f; w6.y = 7.0f; w6.type = MONSTER_TYPE_FLYING; w6.health = 200; w6.isAlive = true; sprites.push_back(w6);
        Sprite w7; w7.x = 25.0f; w7.y = 25.0f; w7.type = MONSTER_TYPE_FLYING; w7.health = 200; w7.isAlive = true; sprites.push_back(w7);

        Sprite w8; w8.x = 26.0f; w8.y = 25.0f; w8.type = TYPE_BARREL; w8.health = 1; w8.isAlive = true; sprites.push_back(w8);
        Sprite w9; w9.x = 26.0f; w9.y = 8.0f; w9.type = TYPE_BARREL; w9.health = 1; w9.isAlive = true; sprites.push_back(w9);
        Sprite w10; w10.x = 41.0f; w10.y = 38.0f; w10.type = TYPE_BARREL; w10.health = 1; w10.isAlive = true; sprites.push_back(w10);
    }

    if (activeMapIndex == 3) {
        Sprite w1; w1.x = 47.0f; w1.y = 30.0f; w1.type = MONSTER_TYPE_GOBLIN; w1.health = 100; w1.isAlive = true; sprites.push_back(w1);

        Sprite w2; w2.x = 6.0f; w2.y = 7.0f; w2.type = MONSTER_TYPE_WALKING; w2.health = 140; w2.isAlive = true; sprites.push_back(w2);
        Sprite w3; w3.x = 8.0f; w3.y = 25.0f; w3.type = MONSTER_TYPE_WALKING; w3.health = 140; w3.isAlive = true; sprites.push_back(w3);
        Sprite w4; w4.x = 24.0f; w4.y = 25.0f; w4.type = MONSTER_TYPE_WALKING; w4.health = 140; w4.isAlive = true; sprites.push_back(w4);
        Sprite w5; w5.x = 8.0f; w5.y = 35.0f; w5.type = MONSTER_TYPE_WALKING; w5.health = 140; w5.isAlive = true; sprites.push_back(w5);
        Sprite w6; w6.x = 24.0f; w6.y = 35.0f; w6.type = MONSTER_TYPE_WALKING; w6.health = 140; w6.isAlive = true; sprites.push_back(w6);
        Sprite w7; w7.x = 40.0f; w7.y = 28.0f; w7.type = MONSTER_TYPE_WALKING; w7.health = 140; w7.isAlive = true; sprites.push_back(w7);
        Sprite w8; w8.x = 40.0f; w8.y = 32.0f; w8.type = MONSTER_TYPE_WALKING; w8.health = 140; w8.isAlive = true; sprites.push_back(w8);
        Sprite w9; w9.x = 3.0f; w9.y = 52.0f; w9.type = MONSTER_TYPE_WALKING; w9.health = 140; w9.isAlive = true; sprites.push_back(w9);

        Sprite w10; w10.x = 24.0f; w10.y = 14.0f; w10.type = MONSTER_TYPE_FLYING; w10.health = 200; w10.isAlive = true; sprites.push_back(w10);
        Sprite w11; w11.x = 16.0f; w11.y = 30.0f; w11.type = MONSTER_TYPE_FLYING; w11.health = 200; w11.isAlive = true; sprites.push_back(w11);
        Sprite w12; w12.x = 16.0f; w12.y = 52.0f; w12.type = MONSTER_TYPE_FLYING; w12.health = 200; w12.isAlive = true; sprites.push_back(w12);
        Sprite w13; w13.x = 8.0f; w13.y = 52.0f; w13.type = MONSTER_TYPE_FLYING; w13.health = 200; w13.isAlive = true; sprites.push_back(w13);
        Sprite w14; w14.x = 24.0f; w14.y = 52.0f; w14.type = MONSTER_TYPE_FLYING; w14.health = 200; w14.isAlive = true; sprites.push_back(w14);
        Sprite w15; w15.x = 40.0f; w15.y = 21.0f; w15.type = MONSTER_TYPE_FLYING; w15.health = 200; w15.isAlive = true; sprites.push_back(w15);
        Sprite w16; w16.x = 40.0f; w16.y = 39.0f; w16.type = MONSTER_TYPE_FLYING; w16.health = 200; w16.isAlive = true; sprites.push_back(w16);

        Sprite w17; w17.x = 40.0f; w17.y = 33.0f; w17.type = TYPE_BARREL; w17.health = 1; w17.isAlive = true; sprites.push_back(w17);
        Sprite w18; w18.x = 40.0f; w18.y = 27.0f; w18.type = TYPE_BARREL; w18.health = 1; w18.isAlive = true; sprites.push_back(w18);
    }

    if (activeMapIndex == 4) {
        Sprite w1; w1.x = 20.0f; w1.y = 16.0f; w1.type = MONSTER_TYPE_GOBLIN; w1.health = 100; w1.isAlive = true; sprites.push_back(w1);
        Sprite w2; w2.x = 28.0f; w2.y = 48.0f; w2.type = MONSTER_TYPE_GOBLIN; w2.health = 100; w2.isAlive = true; sprites.push_back(w2);
        Sprite w3; w3.x = 20.0f; w3.y = 44.0f; w3.type = MONSTER_TYPE_GOBLIN; w3.health = 100; w3.isAlive = true; sprites.push_back(w3);
        Sprite w4; w4.x = 32.0f; w4.y = 28.0f; w4.type = MONSTER_TYPE_GOBLIN; w4.health = 100; w4.isAlive = true; sprites.push_back(w4);
        Sprite w5; w5.x = 48.0f; w5.y = 20.0f; w5.type = MONSTER_TYPE_GOBLIN; w5.health = 100; w5.isAlive = true; sprites.push_back(w5);
        Sprite w6; w6.x = 44.0f; w6.y = 16.0f; w6.type = MONSTER_TYPE_GOBLIN; w6.health = 100; w6.isAlive = true; sprites.push_back(w6);

        Sprite w7; w7.x = 4.0f; w7.y = 8.0f; w7.type = MONSTER_TYPE_FLYING; w7.health = 200; w7.isAlive = true; sprites.push_back(w7);
        Sprite w8; w8.x = 32.0f; w8.y = 16.0f; w8.type = MONSTER_TYPE_FLYING; w8.health = 200; w8.isAlive = true; sprites.push_back(w8);
        Sprite w9; w9.x = 56.0f; w9.y = 40.0f; w9.type = MONSTER_TYPE_FLYING; w9.health = 200; w9.isAlive = true; sprites.push_back(w9);
        Sprite w10; w10.x = 32.0f; w10.y = 44.0f; w10.type = MONSTER_TYPE_FLYING; w10.health = 200; w10.isAlive = true; sprites.push_back(w10);
        Sprite w11; w11.x = 4.0f; w11.y = 55.0f; w11.type = MONSTER_TYPE_FLYING; w11.health = 200; w11.isAlive = true; sprites.push_back(w11);
        Sprite w12; w12.x = 36.0f; w12.y = 56.0f; w12.type = MONSTER_TYPE_FLYING; w12.health = 200; w12.isAlive = true; sprites.push_back(w12);

        Sprite w13; w13.x = 56.0f; w13.y = 4.0f; w13.type = MONSTER_TYPE_WALKING; w13.health = 140; w13.isAlive = true; sprites.push_back(w13);
        Sprite w14; w14.x = 12.0f; w14.y = 12.0f; w14.type = MONSTER_TYPE_WALKING; w14.health = 140; w14.isAlive = true; sprites.push_back(w14);
        Sprite w15; w15.x = 28.0f; w15.y = 12.0f; w15.type = MONSTER_TYPE_WALKING; w14.health = 140; w15.isAlive = true; sprites.push_back(w15);
        Sprite w16; w16.x = 12.0f; w16.y = 32.0f; w16.type = MONSTER_TYPE_WALKING; w16.health = 140; w16.isAlive = true; sprites.push_back(w16);
        Sprite w17; w17.x = 16.0f; w17.y = 36.0f; w17.type = MONSTER_TYPE_WALKING; w17.health = 140; w17.isAlive = true; sprites.push_back(w17);
        Sprite w18; w18.x = 48.0f; w18.y = 44.0f; w18.type = MONSTER_TYPE_WALKING; w18.health = 140; w18.isAlive = true; sprites.push_back(w18);
    }

    if (activeMapIndex == 5) {
        Sprite w1; w1.x = 51.0f; w1.y = 40.0f; w1.type = MONSTER_TYPE_GOBLIN; w1.health = 100; w1.isAlive = true; sprites.push_back(w1);
        Sprite w2; w2.x = 51.0f; w2.y = 41.0f; w2.type = MONSTER_TYPE_GOBLIN; w2.health = 100; w2.isAlive = true; sprites.push_back(w2);
        Sprite w3; w3.x = 51.0f; w3.y = 42.0f; w3.type = MONSTER_TYPE_GOBLIN; w3.health = 100; w3.isAlive = true; sprites.push_back(w3);

        Sprite w4; w4.x = 9.0f; w4.y = 36.0f; w4.type = MONSTER_TYPE_FLYING; w4.health = 200; w4.isAlive = true; sprites.push_back(w4);
        Sprite w5; w5.x = 4.0f; w5.y = 41.0f; w5.type = MONSTER_TYPE_FLYING; w5.health = 200; w5.isAlive = true; sprites.push_back(w5);
        Sprite w6; w6.x = 14.0f; w6.y = 41.0f; w6.type = MONSTER_TYPE_FLYING; w6.health = 200; w6.isAlive = true; sprites.push_back(w6);
        Sprite w7; w7.x = 9.0f; w7.y = 46.0f; w7.type = MONSTER_TYPE_FLYING; w7.health = 200; w7.isAlive = true; sprites.push_back(w7);
        Sprite w8; w8.x = 3.0f; w8.y = 53.0f; w8.type = MONSTER_TYPE_FLYING; w8.health = 200; w8.isAlive = true; sprites.push_back(w8);
        Sprite w9; w9.x = 15.0f; w9.y = 53.0f; w9.type = MONSTER_TYPE_FLYING; w9.health = 200; w9.isAlive = true; sprites.push_back(w9);
        Sprite w10; w10.x = 45.0f; w10.y = 37.0f; w10.type = MONSTER_TYPE_FLYING; w10.health = 200; w10.isAlive = true; sprites.push_back(w10);
        Sprite w11; w11.x = 45.0f; w11.y = 45.0f; w11.type = MONSTER_TYPE_FLYING; w11.health = 200; w11.isAlive = true; sprites.push_back(w11);

        Sprite w12; w12.x = 27.0f; w12.y = 38.0f; w12.type = MONSTER_TYPE_WALKING; w12.health = 140; w12.isAlive = true; sprites.push_back(w12);
        Sprite w13; w13.x = 27.0f; w13.y = 40.0f; w13.type = MONSTER_TYPE_WALKING; w13.health = 140; w13.isAlive = true; sprites.push_back(w13);
        Sprite w14; w14.x = 27.0f; w14.y = 42.0f; w14.type = MONSTER_TYPE_WALKING; w14.health = 140; w14.isAlive = true; sprites.push_back(w14);
        Sprite w15; w15.x = 24.0f; w15.y = 41.0f; w15.type = MONSTER_TYPE_WALKING; w15.health = 140; w15.isAlive = true; sprites.push_back(w15);
        Sprite w16; w16.x = 30.0f; w16.y = 41.0f; w16.type = MONSTER_TYPE_WALKING; w16.health = 140; w16.isAlive = true; sprites.push_back(w16);
        Sprite w17; w17.x = 45.0f; w17.y = 58.0f; w17.type = MONSTER_TYPE_WALKING; w17.health = 140; w17.isAlive = true; sprites.push_back(w17);

        Sprite w18; w18.x = 29.0f; w18.y = 39.0f; w18.type = TYPE_BARREL; w18.health = 1; w18.isAlive = true; sprites.push_back(w18);
        Sprite w19; w19.x = 25.0f; w19.y = 43.0f; w19.type = TYPE_BARREL; w19.health = 1; w19.isAlive = true; sprites.push_back(w19);
        Sprite w20; w20.x = 45.0f; w20.y = 41.0f; w20.type = TYPE_BARREL; w20.health = 1; w20.isAlive = true; sprites.push_back(w20);
        Sprite w21; w21.x = 9.0f; w21.y = 54.0f; w21.type = TYPE_BARREL; w21.health = 1; w21.isAlive = true; sprites.push_back(w21);

        Sprite boss;
        boss.x = 27.0f;
        boss.y = 11.0f;
        boss.type = MONSTER_TYPE_CYBERBOSS;
        boss.health = 2500;
        boss.maxHealth = 2500;
        boss.isAlive = true;
        boss.alternateAttack = false;
        sprites.push_back(boss);
    }

    if (activeMapIndex == 6) {
        Sprite boss;
        boss.x = 49.0f;
        boss.y = 30.0f;

        boss.type = MONSTER_TYPE_FINAL_BOSS;

        boss.health = 3000;
        boss.maxHealth = 3000;
        boss.isAlive = true;
        boss.alternateAttack = false;

        boss.isShielded = true;
        boss.bossPhase = 1;

        sprites.push_back(boss);

        spawnWave(1);
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

void updateBossLogic(float dt, float playerX, float playerY) {
    Sprite* boss = nullptr;
    int minionCount = 0;
    bool itemExists = false;

    for (auto& s : sprites) {
        if (s.type == MONSTER_TYPE_FINAL_BOSS && s.isAlive) {
            boss = &s;
        }
        else if (s.isAlive && !s.isWeapon && s.type != OBJECT_SHIELD_ITEM) {
            minionCount++;
        }
        else if (s.isAlive && s.type == OBJECT_SHIELD_ITEM) {
            itemExists = true;
        }
    }

    if (!boss) return;

    if (boss->health <= 0 && boss->state == STATE_DYING) {
        boss->stateTimer -= dt;
        if (boss->stateTimer <= 0.0f) {
            if (boss->deathSoundCount < 3) {
                playBossPain();
                boss->deathSoundCount++;
                boss->stateTimer = 1.0f;
            }
            else {
                boss->isAlive = false;

                if (activeMapIndex == 6) isGameWon = true;
            }
        }
        return;
    }

    if (boss->isShielded) {
        if (minionCount == 0 && !itemExists) {
            Sprite item;
            item.type = OBJECT_SHIELD_ITEM;
            item.isWeapon = false;
            item.isAlive = true;

            if (boss->bossPhase == 1) {
                item.x = 32.0f;
                item.y = 22.0f;
            }
            else if (boss->bossPhase == 2) {
                item.x = 48.0f;
                item.y = 38.0f;
            }
            else if (boss->bossPhase == 3) {
                item.x = 32.0f;
                item.y = 38.0f;
            }
            else if (boss->bossPhase == 4) {
                item.x = 48.0f;
                item.y = 22.0f;
            }

            sprites.push_back(item);
            std::cout << "PRZEDMIOT TARCZY ZESPAWNOWANY!" << std::endl;
        }
    }
}

bool hitMonster(int index, float hitDamage) {
    if (index < 0 || index >= sprites.size()) return false;
    Sprite& m = sprites[index];
    if (!m.isAlive) return false;
    if (m.type == TYPE_EXPLOSION) return false;

    if (m.type == MONSTER_TYPE_CYBERBOSS) {
        int drops = 8;
        for (int i = 0; i < drops; i++) {
            BloodParticle bp;
            bp.x = m.x;
            bp.y = m.y;
            bp.z = 0.0f;
            bp.life = 1.0f;

            bp.velX = ((float)rand() / RAND_MAX - 0.5f) * 0.15f;
            bp.velY = ((float)rand() / RAND_MAX - 0.5f) * 0.15f;
            bp.velZ = ((float)rand() / RAND_MAX) * 0.1f + 0.05f;

            bloodParticles.push_back(bp);
        }

        m.health -= (int)hitDamage;
        if (!m.hasSeenPlayer) {
            m.hasSeenPlayer = true;
            playBossMusic();
        }
        if (m.health <= 0) {
            m.isAlive = false;
            playBossDeath();
            stopBossMusic();
        }
        return true;
    }

    if (m.type == MONSTER_TYPE_FINAL_BOSS) {
        if (m.isShielded) {
            return false;
        }

        m.health -= (int)hitDamage;

        if (!m.hasSeenPlayer) {
            m.hasSeenPlayer = true;
            playBossMusic();
        }

        float hpPercent = (float)m.health / (float)m.maxHealth;

        if (m.bossPhase == 1 && hpPercent <= 0.75f) {
            m.isShielded = true;
            m.bossPhase = 2;
            spawnWave(2);
        }
        else if (m.bossPhase == 2 && hpPercent <= 0.50f) {
            m.isShielded = true;
            m.bossPhase = 3;
            spawnWave(3);
        }
        else if (m.bossPhase == 3 && hpPercent <= 0.25f) {
            m.isShielded = true;
            m.bossPhase = 4;
            spawnWave(4);
        }

        if (m.health <= 0) {
            m.health = 0;
            m.state = STATE_DYING;
            m.stateTimer = 0.0f;
            m.deathSoundCount = 0;
            stopBossMusic();
        }
        return true;
    }

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

    if (m.type == MONSTER_TYPE_GOBLIN) playGoblinPain();
    else if (m.type == MONSTER_TYPE_FLYING) playFlyingPain();
    else if (m.type == MONSTER_TYPE_WALKING) playWalkerPain();

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

    if (m.type != MONSTER_TYPE_CYBERBOSS && m.type != MONSTER_TYPE_FINAL_BOSS) {
        m.state = STATE_PAIN;
        m.stateTimer = 0.4f;
    }

    if (m.health <= 0) {
        m.isAlive = false;
        m.state = STATE_PAIN;
        return true;
    }
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
            return false;
        }
    }

    return true;
}

void moveMonsters(float playerX, float playerY, float deltaTime, int& playerHealth, int& playerArmor, float& damageAlpha) {
    updateFireParticles(deltaTime);

    if (activeMapIndex == 6) {
        updateBossLogic(deltaTime, playerX, playerY);
    }

    for (auto& m : sprites) {
        if (!m.isAlive || m.isWeapon) continue;

        if (m.type == OBJECT_SHIELD_ITEM) continue;

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
            if (other.type == TYPE_BARREL || other.type == TYPE_EXPLOSION || other.type == OBJECT_SHIELD_ITEM) continue;

            float dx = m.x - other.x;
            float dy = m.y - other.y;
            float dist = std::sqrt(dx * dx + dy * dy);

            float minSpacing = 0.6f;
            if (m.type == MONSTER_TYPE_CYBERBOSS || m.type == MONSTER_TYPE_FINAL_BOSS) minSpacing = 1.2f;

            if (dist < minSpacing && dist > 0.001f) {
                float pushX = dx / dist;
                float pushY = dy / dist;

                float pushStrength = (minSpacing - dist) * 2.0f * deltaTime;

                if (m.type != MONSTER_TYPE_CYBERBOSS && m.type != MONSTER_TYPE_FINAL_BOSS) {
                    float newX = m.x + pushX * pushStrength;
                    float newY = m.y + pushY * pushStrength;
                    if (worldMap[(int)m.y][(int)newX] == 0) m.x = newX;
                    if (worldMap[(int)newY][(int)m.x] == 0) m.y = newY;
                }
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

        if (!canSee && dist > 1.5f && m.type != MONSTER_TYPE_CYBERBOSS && m.type != MONSTER_TYPE_FINAL_BOSS) {
            m.state = STATE_IDLE;
            continue;
        }

        if (!canSee && (m.type == MONSTER_TYPE_CYBERBOSS || m.type == MONSTER_TYPE_FINAL_BOSS) && dist > 3.0f) {
            m.state = STATE_IDLE;
            continue;
        }

        float monsterRadius = 0.3f;
        if (m.type == MONSTER_TYPE_CYBERBOSS || m.type == MONSTER_TYPE_FINAL_BOSS) monsterRadius = 0.6f;

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
                    applyDamage(playerHealth, playerArmor, 15, damageAlpha);
                    m.attackCooldown = 1.0f;
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
        else if (m.type == MONSTER_TYPE_CYBERBOSS) {
            if (m.state == STATE_DYING) continue;

            if (canSee) {
                if (!m.hasSeenPlayer) {
                    m.hasSeenPlayer = true;
                    playBossMusic();
                }
            }

            m.soundTimer -= deltaTime;
            if (m.soundTimer <= 0.0f) {
                playBossIdle();
                m.soundTimer = 6.0f + ((float)rand() / RAND_MAX) * 4.0f;
            }

            if (activeMapIndex == 5) {
                float moveX = 0.0f, moveY = 0.0f;
                if (dist > 3.0f && canSee) {
                    float bossSpeed = 0.015f;
                    moveX = (dx / dist) * bossSpeed;
                    moveY = (dy / dist) * bossSpeed;
                }
                if (worldMap[(int)m.y][(int)(m.x + moveX + (moveX > 0 ? monsterRadius : -monsterRadius))] == 0) m.x += moveX;
                if (worldMap[(int)(m.y + moveY + (moveY > 0 ? monsterRadius : -monsterRadius))][(int)m.x] == 0) m.y += moveY;
            }

            m.attackCooldown -= deltaTime;

            if (m.attackCooldown <= 0.0f && canSee && dist < 18.0f) {

                float dirX = dx / dist;
                float dirY = dy / dist;

                if (m.alternateAttack) {
                    float angle = atan2(dirY, dirX);
                    float spread = 0.25f;
                    float speed = 0.08f;

                    Fireball f1; f1.x = m.x; f1.y = m.y; f1.dirX = cos(angle) * speed; f1.dirY = sin(angle) * speed; f1.active = true;
                    fireballs.push_back(f1);

                    Fireball f2; f2.x = m.x; f2.y = m.y; f2.dirX = cos(angle - spread) * speed; f2.dirY = sin(angle - spread) * speed; f2.active = true;
                    fireballs.push_back(f2);

                    Fireball f3; f3.x = m.x; f3.y = m.y; f3.dirX = cos(angle + spread) * speed; f3.dirY = sin(angle + spread) * speed; f3.active = true;
                    fireballs.push_back(f3);

                    playBossConeAttack();
                    std::cout << "BOSS ATTACK: Cone\n";
                }
                else {
                    float perpX = -dirY;
                    float perpY = dirX;
                    float spacing = 0.6f;
                    float speed = 0.11f;

                    Fireball f1; f1.x = m.x; f1.y = m.y;
                    f1.dirX = dirX * speed; f1.dirY = dirY * speed; f1.active = true;
                    fireballs.push_back(f1);

                    Fireball f2; f2.x = m.x + perpX * spacing; f2.y = m.y + perpY * spacing;
                    f2.dirX = dirX * speed; f2.dirY = dirY * speed; f2.active = true;
                    fireballs.push_back(f2);

                    Fireball f3; f3.x = m.x - perpX * spacing; f3.y = m.y - perpY * spacing;
                    f3.dirX = dirX * speed; f3.dirY = dirY * speed; f3.active = true;
                    fireballs.push_back(f3);

                    playBossLineAttack();
                    std::cout << "BOSS ATTACK: Line\n";
                }

                m.alternateAttack = !m.alternateAttack;
                m.attackCooldown = 3.0f;
            }
        }
        // --- NOWY BOSS (FINAL_BOSS) NA MAPIE 6 ---
        else if (m.type == MONSTER_TYPE_FINAL_BOSS) {
            if (canSee && !m.hasSeenPlayer) {
                m.hasSeenPlayer = true;
                playBossMusic();
            }
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

        if (tile == 1) { fb.active = false; continue; }

        float dx = fb.x - playerX; float dy = fb.y - playerY;
        if (dx * dx + dy * dy < 0.2f) {
            applyDamage(playerHealth, playerArmor, 20, damageAlpha);
            fb.active = false;
        }
    }
    fireballs.erase(std::remove_if(fireballs.begin(), fireballs.end(), [](const Fireball& f) { return !f.active; }), fireballs.end());
}

bool checkCollision(float playerX, float playerY) {
    for (const auto& m : sprites) {
        if (!m.isAlive || m.isWeapon) continue;
        if (m.type == OBJECT_SHIELD_ITEM) continue;

        if (m.type == TYPE_BARREL || m.type == TYPE_EXPLOSION) continue;

        if (m.type == MONSTER_TYPE_GOBLIN || m.type == MONSTER_TYPE_CYBERBOSS || m.type == MONSTER_TYPE_FINAL_BOSS) {
            float dx = playerX - m.x; float dy = playerY - m.y;
            if (dx * dx + dy * dy < COLLISION_RADIUS * COLLISION_RADIUS) return true;
        }
    }
    return false;
}