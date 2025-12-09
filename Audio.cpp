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
    // 1 = Pistol, 2 = Shotgun, 3 = Rifle
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