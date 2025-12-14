#ifndef AUDIO_H
#define AUDIO_H

const int PICKUP_HEALTH = 0;
const int PICKUP_ARMOR = 1;
const int PICKUP_AMMO = 2;
const int PICKUP_WEAPON = 3;
const int PICKUP_KEY = 4;

void initAudio();
void cleanupAudio();

void playShootSound(int weaponType);

void playGoblinIdle();
void playGoblinAttack();
void playGoblinPain();

void playDoorSound();
void playAccessDeniedSound();
void playKeypadClick();

void playStepSound();

void playPickupSound(int type);

void playPlayerPain();

void playDryFireSound();

void playPlayerDeath();

#endif