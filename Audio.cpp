#include "Audio.h"
#include <iostream>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

ma_engine engine;

void initAudio() {
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        std::cout << "Blad inicjalizacji audio!" << std::endl;
    }
}

void cleanupAudio() {
    ma_engine_uninit(&engine);
}

void playShootSound(int weaponType) {
    if (weaponType == 1) {
        ma_engine_play_sound(&engine, "shoot_pistol.wav", NULL);
    }
    else if (weaponType == 2) {
        ma_engine_play_sound(&engine, "shoot_shotgun.wav", NULL);
    }
    else if (weaponType == 3) {
        ma_engine_play_sound(&engine, "shoot_rifle.wav", NULL);
    }
}


void playGoblinIdle() {
    ma_engine_play_sound(&engine, "goblin_idle.wav", NULL);
}

void playGoblinAttack() {
    ma_engine_play_sound(&engine, "goblin_attack.wav", NULL);
}

void playGoblinPain() {
    ma_engine_play_sound(&engine, "goblin_pain.wav", NULL);
}


void playDoorSound() {
    ma_engine_play_sound(&engine, "door_open.wav", NULL);
}

void playAccessDeniedSound() {
    ma_engine_play_sound(&engine, "access_denied.wav", NULL);
}

void playKeypadClick() {
    ma_engine_play_sound(&engine, "keypad_click.wav", NULL);
}

void playStepSound() {
    int r = rand() % 3;
    if (r == 0) ma_engine_play_sound(&engine, "step_1.wav", NULL);
    else if (r == 1) ma_engine_play_sound(&engine, "step_2.wav", NULL);
    else ma_engine_play_sound(&engine, "step_3.wav", NULL);
}

void playPickupSound(int type) {
    if (type == PICKUP_HEALTH) ma_engine_play_sound(&engine, "pickup_health.wav", NULL);
    else if (type == PICKUP_ARMOR) ma_engine_play_sound(&engine, "pickup_armor.wav", NULL);
    else if (type == PICKUP_AMMO) ma_engine_play_sound(&engine, "pickup_ammo.wav", NULL);
    else if (type == PICKUP_WEAPON) ma_engine_play_sound(&engine, "pickup_weapon.wav", NULL);
    else if (type == PICKUP_KEY) ma_engine_play_sound(&engine, "pickup_key.wav", NULL);
}