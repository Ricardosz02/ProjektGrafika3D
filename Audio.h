#ifndef AUDIO_H
#define AUDIO_H

void initAudio();
void cleanupAudio();

void playShootSound(int weaponType);

void playGoblinIdle();
void playGoblinAttack();
void playGoblinPain();

void playDoorSound();
void playAccessDeniedSound();
void playKeypadClick();

#endif