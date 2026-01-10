#include "Audio.h"
#include <iostream>
#include <cstdlib>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

ma_engine engine;
ma_sound menuMusic;
ma_sound bossMusic;
ma_sound levelMusic;

bool isMenuMusicPlaying = false;
bool isBossMusicPlaying = false;
bool isLevelMusicPlaying = false;

void initAudio() {
    ma_result result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        std::cout << "Blad inicjalizacji audio!" << std::endl;
    }

    result = ma_sound_init_from_file(&engine, "menu_theme.mp3", MA_SOUND_FLAG_STREAM, NULL, NULL, &menuMusic);
    if (result != MA_SUCCESS) {
        std::cout << "Nie udalo sie zaladowac muzyki do menu (menu_theme.mp3)!" << std::endl;
    }
    else {
        ma_sound_set_looping(&menuMusic, MA_TRUE);
    }

    result = ma_sound_init_from_file(&engine, "boss_theme.mp3", MA_SOUND_FLAG_STREAM, NULL, NULL, &bossMusic);
    if (result != MA_SUCCESS) {
        std::cout << "AUDIO WARNING: Brak pliku boss_theme.mp3!" << std::endl;
    }
    else {
        ma_sound_set_looping(&bossMusic, MA_TRUE);
    }
}

void cleanupAudio() {
    ma_sound_uninit(&menuMusic);
    ma_sound_uninit(&bossMusic);
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
    stopBossMusic();
    stopLevelMusic();
    ma_engine_play_sound(&engine, "player_die.wav", NULL);
    ma_engine_play_sound(&engine, "game_over.wav", NULL);
}

void playMenuBeep() {
    ma_engine_play_sound(&engine, "menu_blip.wav", NULL);
}

void playMenuMusic() {
    if (!ma_sound_is_playing(&menuMusic)) {
        ma_sound_start(&menuMusic);
    }
}

void stopMenuMusic() {
    if (ma_sound_is_playing(&menuMusic)) {
        ma_sound_stop(&menuMusic);
        ma_sound_seek_to_pcm_frame(&menuMusic, 0);
    }
}

void setGlobalVolume(float volume) {
    ma_engine_set_volume(&engine, volume);
}

void playExplosionSound() {
    ma_engine_play_sound(&engine, "barrel_explode.wav", NULL);
}

void playBossIdle() {
    ma_engine_play_sound(&engine, "boss_idle.wav", NULL);
}

void playBossConeAttack() {
    ma_engine_play_sound(&engine, "boss_cone.wav", NULL);
}

void playBossLineAttack() {
    ma_engine_play_sound(&engine, "boss_line.wav", NULL);
}

void playBossDeath() {
    ma_engine_play_sound(&engine, "boss_death.wav", NULL);
}

void playBossMusic() {
    if (!isBossMusicPlaying) {
        stopMenuMusic();
        stopLevelMusic();
        ma_sound_start(&bossMusic);
        isBossMusicPlaying = true;
    }
}

void stopBossMusic() {
    if (isBossMusicPlaying) {
        ma_sound_stop(&bossMusic);
        ma_sound_seek_to_pcm_frame(&bossMusic, 0);
        isBossMusicPlaying = false;
    }
}

void stopLevelMusic() {
    if (isLevelMusicPlaying) {
        ma_sound_stop(&levelMusic);
        ma_sound_uninit(&levelMusic); // Zwalniamy pamiêæ, bo bêdziemy ³adowaæ inny plik
        isLevelMusicPlaying = false;
    }
}

void playLevelMusic(int mapIndex) {
    // 1. Najpierw wy³¹cz wszystko inne
    stopLevelMusic();
    stopMenuMusic();
    stopBossMusic();

    const char* filename = "";

    // 2. Wybierz plik w zale¿noœci od mapy
    if (mapIndex == 1) filename = "music_map_1.mp3";
    else if (mapIndex == 2) filename = "music_map_2.mp3";
    else if (mapIndex == 3) filename = "music_map_3.mp3";
    else if (mapIndex == 4) filename = "music_map_4.mp3";
    else if (mapIndex == 5) filename = "music_map_5.mp3";
    else if (mapIndex == 6) filename = "music_map_6.mp3";

    // Mapa 6 (i inne nieprzewidziane) nie ma muzyki startowej
    if (filename == "") return;

    // 3. Za³aduj i w³¹cz
    ma_result result = ma_sound_init_from_file(&engine, filename, MA_SOUND_FLAG_STREAM, NULL, NULL, &levelMusic);
    if (result == MA_SUCCESS) {
        ma_sound_set_looping(&levelMusic, MA_TRUE);
        ma_sound_start(&levelMusic);
        isLevelMusicPlaying = true;
        // Opcjonalnie: ustaw g³oœnoœæ (0.5f = 50%)
        ma_sound_set_volume(&levelMusic, 0.3f);
    }
    else {
        std::cout << "AUDIO ERROR: Nie mozna zaladowac " << filename << std::endl;
    }
}