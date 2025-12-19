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
    if (weaponType == 0) {
        ma_engine_play_sound(&engine, "punch.wav", NULL);
    }
    else if (weaponType == 1) {
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

void playFlyingIdle() {
    ma_engine_play_sound(&engine, "flying_idle.wav", NULL);
}

void playFlyingAttack() {
    ma_engine_play_sound(&engine, "flying_attack.wav", NULL);
}

void playFlyingPain() {
    ma_engine_play_sound(&engine, "flying_pain.wav", NULL);
}

void playWalkerIdle() {
    ma_engine_play_sound(&engine, "walker_idle.wav", NULL);
}

void playWalkerPain() {
    ma_engine_play_sound(&engine, "walker_pain.wav", NULL);
}

void playWalkerAttack() {
    static int variant = 0;
    if (variant == 0) {
        ma_engine_play_sound(&engine, "walker_attack_1.wav", NULL);
        variant = 1;
    }
    else {
        ma_engine_play_sound(&engine, "walker_attack_2.wav", NULL);
        variant = 0;
    }
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

void playPlayerPain() {
    ma_engine_play_sound(&engine, "player_pain.wav", NULL);
}

void playDryFireSound() {
    ma_engine_play_sound(&engine, "dry_fire.wav", NULL);
}

void playPlayerDeath() {
    ma_engine_play_sound(&engine, "player_die.wav", NULL);
    ma_engine_play_sound(&engine, "game_over.wav", NULL);
}

void playMenuBeep() {
    ma_engine_play_sound(&engine, "menu_blip.wav", NULL);
}

void setGlobalVolume(float volume) {
    ma_engine_set_volume(&engine, volume);
}