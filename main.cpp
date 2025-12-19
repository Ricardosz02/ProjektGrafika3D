#include "config.h"
#include "Map.h"
#include "Goblin.h"
#include "Weapon.h"
#include "Audio.h"
#include "Menu.h"
#include "stb_image.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

int screenWidth = 1920;
int screenHeight = 1080;

float playerX = 2.5f;
float playerY = 2.5f;
float playerDir = 0.0f;

int playerHealth = 100;
int playerArmor = 0;

float moveSpeed = 0.035f;
float rotSpeed = 0.03f;
bool gameOver = false;

float walkTimer = 0.0f;
float shootTimer = 0.0f;
bool isShooting = false;

float stepTimer = 0.0f;
const float STEP_INTERVAL = 0.5f;

int punchSide = 0;
float maxShootTime = 0.0f;

float damageAlpha = 0.0f;
float flashIntensity = 0.0f;

const float FISTS_DAMAGE = 15.0f;
const float PISTOL_DAMAGE = 25.0f;
const float SHOTGUN_DAMAGE = 100.0f;

struct HitMarker { float x_map, y_map, life; int side; float texX; };
std::vector<HitMarker> hitMarkers;

struct BulletFlash { float x, y, dirX, dirY, life; };
std::vector<BulletFlash> bulletFlashes;

struct Shell {
    float x, y;
    float vx, vy;
    float rotation;
    float vrot;
    float scale;
    int type;
    float life;
};
std::vector<Shell> shells;

extern int (*worldMap)[MAP_WIDTH];
int activeMapIndex = 1;

std::string keypadInput = "";
bool keysPressed[10] = { 0 };

void resetGame() {
    activeMapIndex = 1;
    switchMap(activeMapIndex);
    initMonsters();
    initWeapons();

    playerX = 2.5f;
    playerY = 2.5f;
    playerDir = 0.0f;
    playerHealth = 100;
    playerArmor = 0;
    gameOver = false;

    ammoPistol = 0;
    ammoShotgun = 0;
    ammoRifle = 0;
    currentWeapon = 0;
    hasPistol = false;
    hasShotgun = false;
    hasRifle = false;
    hasGreenKey = false;
    hasRedKey = false;

    hitMarkers.clear();
    bulletFlashes.clear();
    shells.clear();
    damageAlpha = 0.0f;

    std::cout << "--- NOWA GRA ---" << std::endl;
}

void updateSprites(float playerX, float playerY, int& health, int& armor) {
    if (!sprites.empty()) {
        moveMonsters(playerX, playerY, 0.016f, health, armor, damageAlpha);
        removeDeadMonsters();
    }
    updateBloodParticles(0.016f);
}

void updateShells(float dt) {
    for (auto& s : shells) {
        s.x += s.vx * dt;
        s.y += s.vy * dt;
        s.vy -= 3.0f * dt;
        s.rotation += s.vrot * dt;
        s.life -= dt;
    }
    shells.erase(std::remove_if(shells.begin(), shells.end(),
        [](const Shell& s) { return s.life <= 0.0f || s.y < -1.2f; }), shells.end());
}

const char* vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
out vec3 ourColor; out vec2 TexCoord;
void main() { gl_Position = vec4(aPos, 0.0, 1.0); ourColor = aColor; TexCoord = aTexCoord; }
)glsl";

const char* fragmentShaderSource = R"glsl(
#version 330 core
in vec3 ourColor; in vec2 TexCoord; out vec4 FragColor;

uniform sampler2D wallTexture;       // 0
uniform sampler2D monsterTexture;    // 1
uniform sampler2D pistolTexture;     // 2
uniform sampler2D fontTexture;       // 3
uniform sampler2D hitTexture;        // 4
uniform sampler2D floorTexture;      // 5
uniform sampler2D ceilingTexture;    // 6
uniform sampler2D weaponViewTexture; // 7
uniform sampler2D shotgunTexture;    // 8
uniform sampler2D shotgunViewTexture;// 9
uniform sampler2D ammoPistolTexture; // 10
uniform sampler2D ammoShotgunTexture;// 11
uniform sampler2D fly1Texture;       // 12
uniform sampler2D fly2Texture;       // 13
uniform sampler2D fly3Texture;       // 14
uniform sampler2D fireballTexture;   // 15
uniform sampler2D wWalk1Tex;         // 16
uniform sampler2D wWalk2Tex;         // 17
uniform sampler2D wWalk3Tex;         // 18
uniform sampler2D wHitTex;           // 19
uniform sampler2D wFight1Tex;        // 20
uniform sampler2D wFight2Tex;        // 21
uniform sampler2D medkitTexture;     // 22
uniform sampler2D fistsTexture;      // 23
uniform sampler2D armorTexture;      // 24
uniform sampler2D pistolShootTex;    // 25
uniform sampler2D shotgunShootTex;   // 26
uniform sampler2D bloodScreenTex;    // 27
uniform sampler2D deathTex;          // 28
uniform sampler2D fistR1Tex;         // 29
uniform sampler2D fistR2Tex;         // 30
uniform sampler2D fistL1Tex;         // 31
uniform sampler2D fistL2Tex;         // 32
uniform sampler2D doorTexture;       // 33
uniform sampler2D holePistolTex;     // 34
uniform sampler2D holeShotgunTex;    // 35
uniform sampler2D bloodPartTex;      // 36
uniform sampler2D keyGreenTex;       // 37
uniform sampler2D keyRedTex;         // 38
uniform sampler2D dGreenL;           // 39
uniform sampler2D dGreenO;           // 40
uniform sampler2D dRedL;             // 41
uniform sampler2D dRedO;             // 42
uniform sampler2D dDualL;            // 43
uniform sampler2D dDualO;            // 44
uniform sampler2D dCodeL;            // 45
uniform sampler2D dCodeO;            // 46
uniform sampler2D keypadRed;         // 47
uniform sampler2D keypadGreen;       // 48
uniform sampler2D rifleTex;          // 49
uniform sampler2D rifleViewTex;      // 50
uniform sampler2D rifleShootTex;     // 51
uniform sampler2D ammoRifleTex;      // 52
uniform sampler2D hudTexture;        // 53
uniform sampler2D shellTex;          // 54
uniform sampler2D shellShotgunTex;   // 55
uniform sampler2D menuBgTex;         // 56
uniform sampler2D logoTex;           // 57

uniform bool useTexture; uniform float playerDir; uniform vec2 playerPos; uniform float screenWidth; uniform float screenHeight;
uniform float damageIntensity;
uniform float horizon;
uniform float flashIntensity;

void main() {
    if (useTexture) {
        vec4 texColor;
        bool isBlood = false;
        vec3 bloodRed = vec3(0.569, 0.075, 0.110);

        if (ourColor.b > 130.9)      texColor = texture(shellShotgunTex, TexCoord);
        else if (ourColor.b > 129.9) texColor = texture(shellTex, TexCoord);
        else if (ourColor.b > 124.9) texColor = texture(hudTexture, TexCoord);
        else if (ourColor.b > 119.9) texColor = texture(keypadGreen, TexCoord);
        else if (ourColor.b > 118.9) texColor = texture(keypadRed, TexCoord);
        else if (ourColor.b > 110.9) texColor = texture(dCodeO, TexCoord);
        else if (ourColor.b > 109.9) texColor = texture(dCodeL, TexCoord);
        else if (ourColor.b > 107.9) texColor = texture(dDualO, TexCoord);
        else if (ourColor.b > 106.9) texColor = texture(dDualL, TexCoord);
        else if (ourColor.b > 105.9) texColor = texture(dRedO, TexCoord);
        else if (ourColor.b > 104.9) texColor = texture(dRedL, TexCoord);
        else if (ourColor.b > 103.9) texColor = texture(dGreenO, TexCoord);
        else if (ourColor.b > 102.9) texColor = texture(dGreenL, TexCoord);
        else if (ourColor.b > 101.9) texColor = texture(keyRedTex, TexCoord);
        else if (ourColor.b > 100.9) texColor = texture(keyGreenTex, TexCoord);
        else if (ourColor.b > 98.9) texColor = texture(holeShotgunTex, TexCoord);
        else if (ourColor.b > 97.9) texColor = texture(holePistolTex, TexCoord);
        else if (ourColor.b > 95.9) {
             float dist = distance(TexCoord, vec2(0.5, 0.5));
             if (dist > 0.5) discard;
             float alpha = 0.6 * (1.0 - dist * 2.0);
             texColor = vec4(0.0, 0.0, 0.0, alpha);
        }
        else if (ourColor.b > 94.9) {
             texColor = vec4(0.0, 0.0, 0.0, 0.95); 
        }
        else if (ourColor.b > 89.9) texColor = texture(wallTexture, TexCoord);
        else if (ourColor.b > 79.9) texColor = texture(bloodPartTex, TexCoord);
        
        else if (ourColor.b > 60.9) texColor = texture(logoTex, TexCoord);
        else if (ourColor.b > 59.9) texColor = texture(menuBgTex, TexCoord);
        
        else if (ourColor.b > 52.9) texColor = texture(ammoRifleTex, TexCoord);
        else if (ourColor.b > 51.9) texColor = texture(rifleShootTex, TexCoord);
        else if (ourColor.b > 50.9) texColor = texture(rifleViewTex, TexCoord);
        else if (ourColor.b > 49.9) texColor = texture(rifleTex, TexCoord);
        else if (ourColor.b > 35.9) texColor = texture(doorTexture, TexCoord);
        else if (ourColor.b > 31.9) {
             vec4 wCol;
             if (ourColor.b > 34.9)      wCol = texture(fistL2Tex, TexCoord);
             else if (ourColor.b > 33.9) wCol = texture(fistL1Tex, TexCoord);
             else if (ourColor.b > 32.9) wCol = texture(fistR2Tex, TexCoord);
             else                        wCol = texture(fistR1Tex, TexCoord);
             texColor = vec4(mix(wCol.rgb, bloodRed, damageIntensity * 0.4), wCol.a);
        }
        else if (ourColor.b > 30.9) texColor = texture(wallTexture, TexCoord);
        else if (ourColor.b > 29.9) {
            vec4 imgColor = texture(bloodScreenTex, TexCoord);
            isBlood = true;
            texColor.rgb = mix(bloodRed, imgColor.rgb, imgColor.a);
            float finalAlpha = max(0.6, imgColor.a);
            texColor.a = finalAlpha * damageIntensity;
            texColor = vec4(texColor.rgb, texColor.a);
        }
        else if (ourColor.b > 26.9) {
            vec4 wCol = texture(shotgunShootTex, TexCoord);
            texColor = vec4(mix(wCol.rgb, bloodRed, damageIntensity * 0.4), wCol.a);
        }
        else if (ourColor.b > 25.9) {
            vec4 wCol = texture(pistolShootTex, TexCoord);
            texColor = vec4(mix(wCol.rgb, bloodRed, damageIntensity * 0.4), wCol.a);
        }
        else if (ourColor.b > 24.9) texColor = texture(armorTexture, TexCoord);
        else if (ourColor.b > 23.9) {
            vec4 wCol = texture(fistsTexture, TexCoord);
            texColor = vec4(mix(wCol.rgb, bloodRed, damageIntensity * 0.4), wCol.a);
        }
        else if (ourColor.b > 22.9) texColor = texture(medkitTexture, TexCoord);
        else if (ourColor.b > 21.9) texColor = texture(wFight2Tex, TexCoord);
        else if (ourColor.b > 20.9) texColor = texture(wFight1Tex, TexCoord);
        else if (ourColor.b > 19.9) texColor = texture(wHitTex, TexCoord);
        else if (ourColor.b > 18.9) texColor = texture(wWalk3Tex, TexCoord);
        else if (ourColor.b > 17.9) texColor = texture(wWalk2Tex, TexCoord);
        else if (ourColor.b > 16.9) texColor = texture(wWalk1Tex, TexCoord);
        else if (ourColor.b > 15.9) texColor = texture(fireballTexture, TexCoord);
        else if (ourColor.b > 14.9) texColor = texture(fly3Texture, TexCoord);
        else if (ourColor.b > 13.9) texColor = texture(fly2Texture, TexCoord);
        else if (ourColor.b > 12.9) texColor = texture(fly1Texture, TexCoord);
        else if (ourColor.b > 11.9) texColor = texture(ammoShotgunTexture, TexCoord);
        else if (ourColor.b > 10.9) texColor = texture(ammoPistolTexture, TexCoord);
        else if (ourColor.b > 9.9) {
            vec4 wCol = texture(shotgunViewTexture, TexCoord);
            texColor = vec4(mix(wCol.rgb, bloodRed, damageIntensity * 0.4), wCol.a);
        }
        else if (ourColor.b > 8.9)  texColor = texture(shotgunTexture, TexCoord);
        else if (ourColor.b > 7.9)  texColor = vec4(0.2, 0.2, 0.2, 1.0);
        else if (ourColor.b > 6.9)  {
             vec4 wCol = texture(weaponViewTexture, TexCoord);
             texColor = vec4(mix(wCol.rgb, bloodRed, damageIntensity * 0.4), wCol.a);
        }
        else if (ourColor.b > 5.9) {
            float p = gl_FragCoord.y - (screenHeight / 2.0);
            if (p < 1.0) p = 1.0;
            float posZ = 0.5 * screenHeight; float rowDistance = posZ / p;
            float cameraX = (gl_FragCoord.x / screenWidth) * 2.0 - 1.0;
            float rayDirX = cos(playerDir) + cos(playerDir+1.5708)*cameraX;
            float rayDirY = sin(playerDir) + sin(playerDir+1.5708)*cameraX;
            vec2 ceilPos = playerPos + rowDistance * vec2(rayDirX, rayDirY);
            float light = min(1.0, 3.5 / rowDistance);
            texColor = texture(ceilingTexture, ceilPos) * vec4(light, light, light, 1.0);
        } else if (ourColor.b > 4.9) {
            float p = (screenHeight / 2.0) - gl_FragCoord.y;
            if (p < 1.0) p = 1.0;
            float posZ = 0.5 * screenHeight; float rowDistance = posZ / p;
            float cameraX = (gl_FragCoord.x / screenWidth) * 2.0 - 1.0;
            float rayDirX = cos(playerDir) + cos(playerDir+1.5708)*cameraX;
            float rayDirY = sin(playerDir) + sin(playerDir+1.5708)*cameraX;
            vec2 floorPos = playerPos + rowDistance * vec2(rayDirX, rayDirY);
            float light = min(1.0, 3.5 / rowDistance);
            texColor = texture(floorTexture, floorPos) * vec4(light, light, light, 1.0);
        } else if (ourColor.b > 3.9) texColor = texture(hitTexture, TexCoord);
        else if (ourColor.b > 2.9) texColor = texture(fontTexture, TexCoord);
        else if (ourColor.b > 1.9) texColor = texture(pistolTexture, TexCoord);
        else if (ourColor.b > 0.99) texColor = texture(monsterTexture, TexCoord);
        else texColor = texture(wallTexture, TexCoord);

        if (!isBlood && texColor.a < 0.1) discard;

        if (!isBlood) FragColor = texColor * vec4(ourColor.r, ourColor.r, ourColor.r, 1.0);
        else FragColor = texColor;
    } else { FragColor = vec4(ourColor, 1.0); }
}
)glsl";

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type); glShaderSource(shader, 1, &source, nullptr); glCompileShader(shader);
    int success; glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) { char info[512]; glGetShaderInfoLog(shader, 512, nullptr, info); std::cerr << "Shader error: " << info << std::endl; }
    return shader;
}

GLuint loadTexture(const char* path) {
    GLuint textureID; glGenTextures(1, &textureID); glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int width, height, nrChannels; stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 4);
    if (data) { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); glGenerateMipmap(GL_TEXTURE_2D); }
    else { std::cerr << "Blad ladowania: " << path << std::endl; }
    stbi_image_free(data); stbi_set_flip_vertically_on_load(false); return textureID;
}

void drawGameOverText(std::vector<float>& v) {
    float cx = 0, cy = 0, w = 0.8, h = 0.2;
    v.insert(v.end(), { cx - w, cy + h, 1,0,0, 0,0, cx + w, cy + h, 1,0,0, 0,0, cx + w, cy - h, 1,0,0, 0,0, cx - w, cy + h, 1,0,0, 0,0, cx + w, cy - h, 1,0,0, 0,0, cx - w, cy - h, 1,0,0, 0,0 });
}

void drawQuad2D(std::vector<float>& v, float x, float y, float w, float h, float cB) {
    float x1 = x - w, y1 = y - h, x2 = x + w, y2 = y + h;
    v.insert(v.end(), { x1,y2,1,1,cB,0,1, x2,y2,1,1,cB,1,1, x2,y1,1,1,cB,1,0, x1,y2,1,1,cB,0,1, x2,y1,1,1,cB,1,0, x1,y1,1,1,cB,0,0 });
}

void drawRotatedQuad2D(std::vector<float>& v, float x, float y, float w, float h, float angle, float cB) {
    float halfW = w / 2.0f;
    float halfH = h / 2.0f;
    float localX[4] = { -halfW,  halfW,  halfW, -halfW };
    float localY[4] = { halfH,  halfH, -halfH, -halfH };
    float cosA = cos(angle); float sinA = sin(angle);
    float finalX[4], finalY[4];

    for (int i = 0; i < 4; i++) {
        finalX[i] = x + (localX[i] * cosA - localY[i] * sinA);
        finalY[i] = y + (localX[i] * sinA + localY[i] * cosA);
    }
    v.insert(v.end(), { finalX[3], finalY[3], 1,1, cB, 0, 1 }); v.insert(v.end(), { finalX[2], finalY[2], 1,1, cB, 1, 1 }); v.insert(v.end(), { finalX[1], finalY[1], 1,1, cB, 1, 0 });
    v.insert(v.end(), { finalX[3], finalY[3], 1,1, cB, 0, 1 }); v.insert(v.end(), { finalX[1], finalY[1], 1,1, cB, 1, 0 }); v.insert(v.end(), { finalX[0], finalY[0], 1,1, cB, 0, 0 });
}

void drawChar(std::vector<float>& v, float x, float y, float s, char c) {
    if (c < 32 || c>126) return; int col = (c - 32) % 16, row = (c - 32) / 16; float cw = 1.0f / 16, ch = 1.0f / 6;
    float u1 = col * cw, v1 = 1 - (row + 1) * ch, u2 = (col + 1) * cw, v2 = 1 - row * ch;
    v.insert(v.end(), { x,y + s,1,1,3,u1,v2, x + s,y + s,1,1,3,u2,v2, x + s,y,1,1,3,u2,v1, x,y + s,1,1,3,u1,v2, x + s,y,1,1,3,u2,v1, x,y,1,1,3,u1,v1 });
}

void drawText(std::vector<float>& v, float sx, float sy, float s, const std::string& t) {
    float cx = sx; for (char c : t) { drawChar(v, cx, sy, s, c); cx += s * 0.65f; }
}

int main() {
    GLFWwindow* window; glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(screenWidth, screenHeight, "Mini DOOM", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window); if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint p = glCreateProgram();
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource), fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    glAttachShader(p, vs); glAttachShader(p, fs); glLinkProgram(p); glDeleteShader(vs); glDeleteShader(fs);

    GLuint VAO, VBO; glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * screenWidth * 40 * 7 * 2, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float))); glEnableVertexAttribArray(2);

    GLuint t[58];
    t[0] = loadTexture("wall.png"); t[1] = loadTexture("monster.png"); t[2] = loadTexture("pistol.png");
    t[3] = loadTexture("font.png"); t[4] = loadTexture("hit.png"); t[5] = loadTexture("floor.png");
    t[6] = loadTexture("ceiling.png"); t[7] = loadTexture("pistol_view_128.png"); t[8] = loadTexture("shotgun.png");
    t[9] = loadTexture("shotgun_view_128.png"); t[10] = loadTexture("ammunition_pistol.png"); t[11] = loadTexture("ammunition_shotgun.png");
    t[12] = loadTexture("monster_flying_1.png"); t[13] = loadTexture("monster_flying_2.png"); t[14] = loadTexture("monster_flying_3.png");
    t[15] = loadTexture("fireball.png"); t[16] = loadTexture("monster_walk_1.png"); t[17] = loadTexture("monster_walk_2.png");
    t[18] = loadTexture("monster_walk_3.png"); t[19] = loadTexture("monster_walk_hit_5.png"); t[20] = loadTexture("monster_walk_fight_1.png");
    t[21] = loadTexture("monster_walk_fight_2.png"); t[22] = loadTexture("mecidal.png"); t[23] = loadTexture("hand_idle.png");
    t[24] = loadTexture("bulletproof_armor.png");
    t[25] = loadTexture("pistol_view_128_shoot.png");
    t[26] = loadTexture("shotgun_view_128_shoot.png");
    t[27] = loadTexture("blood_screen_damage.png");
    t[28] = loadTexture("death_word.png");
    t[29] = loadTexture("hand_attack_right_1.png");
    t[30] = loadTexture("hand_attack_right_2.png");
    t[31] = loadTexture("hand_attack_left_1.png");
    t[32] = loadTexture("hand_attack_left_2.png");
    t[33] = loadTexture("door.png");
    t[34] = loadTexture("bullethole_pistol.png");
    t[35] = loadTexture("bullethole_shotgun.png");
    t[36] = loadTexture("blood_particle.png");
    t[37] = loadTexture("green_card.png");
    t[38] = loadTexture("red_card.png");
    t[39] = loadTexture("door_green_locked.png");
    t[40] = loadTexture("door_green_open.png");
    t[41] = loadTexture("door_red_locked.png");
    t[42] = loadTexture("door_red_open.png");
    t[43] = loadTexture("door_dual_locked.png");
    t[44] = loadTexture("door_dual_open.png");
    t[45] = loadTexture("door_code_locked.png");
    t[46] = loadTexture("door_code_open.png");
    t[47] = loadTexture("keypad_ui_red.png");
    t[48] = loadTexture("keypad_ui_green.png");
    t[49] = loadTexture("rifle.png");
    t[50] = loadTexture("rifle_view.png");
    t[51] = loadTexture("rifle_view_shoot.png");
    t[52] = loadTexture("ammunition_rifle.png");
    t[53] = loadTexture("hud_overlay.png");
    t[54] = loadTexture("shell.png");
    t[55] = loadTexture("shell_shotgun.png");
    t[56] = loadTexture("menu_bg.png");
    t[57] = loadTexture("logo.png");

    initAudio();

    glUseProgram(p);
    GLint useTextureLoc = glGetUniformLocation(p, "useTexture");
    glUniform1i(useTextureLoc, 1);
    GLint dmgIntLoc = glGetUniformLocation(p, "damageIntensity");

    const char* names[] = { "wallTexture","monsterTexture","pistolTexture","fontTexture","hitTexture","floorTexture","ceilingTexture",
        "weaponViewTexture","shotgunTexture","shotgunViewTexture","ammoPistolTexture","ammoShotgunTexture","fly1Texture","fly2Texture",
        "fly3Texture","fireballTexture","wWalk1Tex","wWalk2Tex","wWalk3Tex","wHitTex","wFight1Tex","wFight2Tex","medkitTexture",
        "fistsTexture", "armorTexture", "pistolShootTex", "shotgunShootTex", "bloodScreenTex", "deathTex",
        "fistR1Tex", "fistR2Tex", "fistL1Tex", "fistL2Tex", "doorTexture", "holePistolTex", "holeShotgunTex", "bloodPartTex",
        "keyGreenTex", "keyRedTex",
        "dGreenL", "dGreenO", "dRedL", "dRedO", "dDualL", "dDualO",
        "dCodeL", "dCodeO", "keypadRed", "keypadGreen",
        "rifleTex", "rifleViewTex", "rifleShootTex", "ammoRifleTex", "hudTexture",
        "shellTex", "shellShotgunTex",
        "menuBgTex", "logoTex" };

    for (int i = 0; i < 58; i++) glUniform1i(glGetUniformLocation(p, names[i]), i);
    for (int i = 0; i < 58; i++) { glActiveTexture(GL_TEXTURE0 + i); glBindTexture(GL_TEXTURE_2D, t[i]); }
    glActiveTexture(GL_TEXTURE0);

    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
    std::vector<float> zBuffer(screenWidth), vertices;

    GameState gameState = MENU;
    MenuContext menuCtx;
    static bool escPressedLastFrame = false;

    activeMapIndex = 1; switchMap(activeMapIndex); initMonsters(); initWeapons();

    static bool spacePressedLastFrame = false;
    static bool ePressedLastFrame = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        float finalIntensity = 0.0f;

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            if (!escPressedLastFrame) {
                if (gameState == PLAYING) gameState = MENU;
                else if (gameState == MENU) gameState = PLAYING;
                escPressedLastFrame = true;
            }
        }
        else escPressedLastFrame = false;

        bool shouldClose = false;

        if (gameState == MENU) {
            updateMenu(window, gameState, menuCtx, shouldClose, resetGame, screenWidth, screenHeight);
            if (shouldClose) glfwSetWindowShouldClose(window, true);
        }
        else if (gameState == PLAYING) {
            updateShells(0.016f);

            if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !ePressedLastFrame) {
                float checkX = playerX + cos(playerDir) * 1.0f;
                float checkY = playerY + sin(playerDir) * 1.0f;
                openDoorAt((int)checkX, (int)checkY);
                ePressedLastFrame = true;
            }
            else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) ePressedLastFrame = false;

            updateDoors(0.016f);

            float healthAlpha = 0.0f;
            if (playerHealth <= 50) {
                float factor = (50.0f - (float)playerHealth) / 50.0f;
                healthAlpha = 0.2f + factor * 0.7f;
                if (playerHealth <= 25) healthAlpha = std::max(healthAlpha, 0.6f + factor * 0.4f);
            }
            if (damageAlpha > 0.0f) {
                damageAlpha -= 0.016f;
                if (damageAlpha < 0.0f) damageAlpha = 0.0f;
            }

            finalIntensity = std::max(damageAlpha, healthAlpha);
            glUniform1f(dmgIntLoc, finalIntensity);

            if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) currentWeapon = 0;
            if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) if (hasPistol) currentWeapon = 1;
            if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) if (hasShotgun) currentWeapon = 2;
            if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) if (hasRifle) currentWeapon = 3;

            if (shootTimer > 0.0f) {
                shootTimer -= 0.016f;
                if (shootTimer <= 0.0f) isShooting = false;
            }

            bool isRifle = (currentWeapon == 3);
            bool triggerPressed = false;
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !isKeypadActive) {
                if (isRifle) triggerPressed = true;
                else if (!spacePressedLastFrame) triggerPressed = true;
            }

            if (triggerPressed) {
                bool canShoot = (currentWeapon == 0) || (currentWeapon == 1 && ammoPistol > 0) || (currentWeapon == 2 && ammoShotgun > 0) || (currentWeapon == 3 && ammoRifle > 0);
                if (isRifle && shootTimer > 0.0f) canShoot = false;
                bool hasAmmo = true;
                if (currentWeapon == 1 && ammoPistol <= 0) hasAmmo = false;
                if (currentWeapon == 2 && ammoShotgun <= 0) hasAmmo = false;
                if (currentWeapon == 3 && ammoRifle <= 0) hasAmmo = false;
                if (currentWeapon == 0) hasAmmo = true;

                if (canShoot && hasAmmo) {
                    if (currentWeapon == 1) ammoPistol--; else if (currentWeapon == 2) ammoShotgun--; else if (currentWeapon == 3) ammoRifle--;
                    playShootSound(currentWeapon);
                    if (currentWeapon > 0) bulletFlashes.push_back({ playerX + cos(playerDir) * 0.2f, playerY + sin(playerDir) * 0.2f, cos(playerDir), sin(playerDir), 2.0f });

                    if (currentWeapon > 0) {
                        Shell s; s.x = 0.35f; s.y = -0.5f; s.life = 2.0f; s.rotation = 0.0f;
                        float rnd = (float)rand() / RAND_MAX;
                        s.vx = 0.2f + rnd * 0.2f; s.vy = 1.0f + rnd * 0.4f; s.vrot = 5.0f + rnd * 5.0f;
                        if (currentWeapon == 2) { s.type = 1; s.scale = 0.08f; s.vx = 0.15f + rnd * 0.1f; }
                        else { s.type = 0; s.scale = 0.05f; }
                        shells.push_back(s);
                    }

                    if (currentWeapon == 1) { isShooting = true; shootTimer = 0.15f; }
                    else if (currentWeapon == 2) { isShooting = true; shootTimer = 0.40f; }
                    else if (currentWeapon == 3) { isShooting = true; shootTimer = 0.10f; }
                    else if (currentWeapon == 0) { isShooting = true; shootTimer = 0.6f; maxShootTime = shootTimer; punchSide = 1 - punchSide; }

                    int pellets = 1; float spread = 0.0f; float spreadY = 0.0f;
                    if (currentWeapon == 1) { pellets = 1; spread = 0.04f; spreadY = 0.05f; }
                    if (currentWeapon == 2) { pellets = 8; spread = 0.15f; spreadY = 0.30f; }
                    if (currentWeapon == 3) { pellets = 1; spread = 0.06f; spreadY = 0.06f; }
                    if (currentWeapon == 0) { pellets = 1; spread = 0.0f;  spreadY = 0.0f; }

                    for (int p = 0; p < pellets; p++) {
                        float angleOffset = ((float)rand() / RAND_MAX - 0.5f) * spread;
                        float bulletDir = playerDir + angleOffset;
                        float randHeight = ((float)rand() / RAND_MAX - 0.5f) * spreadY;
                        float rayDX = cos(bulletDir); float rayDY = sin(bulletDir);
                        float dist = 1e30f; int hSide = 0, hMX = -1, hMY = -1; float hTX = 0;
                        int mX = (int)playerX, mY = (int)playerY; float dDX = abs(1 / rayDX), dDY = abs(1 / rayDY);
                        int sX = (rayDX < 0) ? -1 : 1, sY = (rayDY < 0) ? -1 : 1; float sDX = (rayDX < 0) ? (playerX - mX) * dDX : (mX + 1 - playerX) * dDX, sDY = (rayDY < 0) ? (playerY - mY) * dDY : (mY + 1 - playerY) * dDY;
                        int hit = 0, side;
                        while (!hit) { if (sDX < sDY) { sDX += dDX; mX += sX; side = 0; } else { sDY += dDY; mY += sY; side = 1; } if (worldMap[mY][mX] == 1) hit = 1; }
                        dist = (side == 0) ? (mX - playerX + (1 - sX) / 2.0f) / rayDX : (mY - playerY + (1 - sY) / 2.0f) / rayDY;
                        if (side == 0)hTX = playerY + dist * rayDY; else hTX = playerX + dist * rayDX; hTX -= floor(hTX);
                        if ((side == 0 && rayDX > 0) || (side == 1 && rayDY < 0)) hTX = 1 - hTX;
                        hSide = side; hMX = mX; hMY = mY;

                        float bestDist = 1e30f; int bestIdx = -1;
                        for (int i = 0; i < sprites.size(); ++i) {
                            if (!sprites[i].isAlive || sprites[i].isWeapon) continue;
                            float sX = sprites[i].x - playerX, sY = sprites[i].y - playerY;
                            float iD = 1.0f / (cos(bulletDir + M_PI / 2) * sin(bulletDir) - cos(bulletDir) * sin(bulletDir + M_PI / 2));
                            float tY = iD * (-sin(bulletDir + M_PI / 2) * sX + cos(bulletDir + M_PI / 2) * sY);
                            float tX = iD * (sin(bulletDir) * sX - cos(bulletDir) * sY);
                            if (tY > 0 && abs(tX / tY) < 0.1f && tY < bestDist) { bestDist = tY; bestIdx = i; }
                        }
                        float dmg = (currentWeapon == 0) ? FISTS_DAMAGE : (currentWeapon == 1 ? PISTOL_DAMAGE : (currentWeapon == 3 ? 20.0f : SHOTGUN_DAMAGE));
                        if (currentWeapon == 2) dmg /= 8.0f;
                        float rng = (currentWeapon == 0) ? 1.0f : 100.0f;
                        if (bestIdx != -1 && bestDist < rng && bestDist < dist) hitMonster(bestIdx, dmg);
                        else if (currentWeapon > 0 && hit) hitMarkers.push_back({ (float)hMX + 0.5f, (float)hMY + 0.5f, 100.0f, hSide, hTX });

                        bool hitWall = hit && (bestIdx == -1 || bestDist > dist);
                        if (hitWall && currentWeapon > 0) {
                            WallDecal newDecal; newDecal.x = hMX; newDecal.y = hMY; newDecal.side = hSide; newDecal.hitX = hTX; newDecal.hitY = randHeight;
                            newDecal.type = (currentWeapon == 2) ? 99 : 98;
                            wallDecals.push_back(newDecal); if (wallDecals.size() > 50) wallDecals.erase(wallDecals.begin());
                        }
                    }
                }
                else if (!hasAmmo) {
                    if (isRifle) { if (!spacePressedLastFrame) { playDryFireSound(); spacePressedLastFrame = true; } }
                    else playDryFireSound();
                    if (currentWeapon == 3) shootTimer = 0.1f; else shootTimer = 0.2f;
                    isShooting = false;
                }
                if (!isRifle) spacePressedLastFrame = true;
            }
            else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) spacePressedLastFrame = false;

            if (!gameOver && !isKeypadActive) {
                float currentSpeed = moveSpeed;
                if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) currentSpeed *= 2.0f;
                float mS = currentSpeed; bool moving = false;
                auto tryMove = [&](float moveStep) {
                    float dx = cos(playerDir) * moveStep; float dy = sin(playerDir) * moveStep; float collisionRadius = 0.25f;
                    float nextX = playerX + dx; float checkX = nextX + (dx > 0 ? collisionRadius : -collisionRadius);
                    int typeX = worldMap[(int)playerY][(int)checkX];
                    bool doorBlockX = false;
                    if (typeX == 2 || (typeX >= 3 && typeX <= 5) || typeX == 8) { Door* d = getDoor((int)checkX, (int)playerY); if (d && d->openAmount < 0.7f) doorBlockX = true; }
                    if (typeX == 0 || ((typeX >= 2 && typeX <= 5 || typeX == 8) && !doorBlockX)) { playerX = nextX; moving = true; }
                    else if (typeX == 9) {
                        if (activeMapIndex == 1) { switchMap(2); activeMapIndex = 2; playerX = 2.5f; playerY = 2.5f; initMonsters(); initWeapons(); }
                        else { switchMap(1); activeMapIndex = 1; playerX = 2.5f; playerY = 7.5f; initMonsters(); initWeapons(); } return;
                    }
                    float nextY = playerY + dy; float checkY = nextY + (dy > 0 ? collisionRadius : -collisionRadius);
                    int typeY = worldMap[(int)checkY][(int)playerX];
                    bool doorBlockY = false;
                    if (typeY == 2 || (typeY >= 3 && typeY <= 5) || typeY == 8) { Door* d = getDoor((int)playerX, (int)checkY); if (d && d->openAmount < 0.7f) doorBlockY = true; }
                    if (typeY == 0 || ((typeY >= 2 && typeY <= 5 || typeY == 8) && !doorBlockY)) { playerY = nextY; moving = true; }
                    else if (typeY == 9) {
                        if (activeMapIndex == 1) { switchMap(2); activeMapIndex = 2; playerX = 2.5f; playerY = 2.5f; initMonsters(); initWeapons(); }
                        else { switchMap(1); activeMapIndex = 1; playerX = 2.5f; playerY = 7.5f; initMonsters(); initWeapons(); }
                    }
                    };
                if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) tryMove(mS);
                if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) tryMove(-mS);
                if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) playerDir -= rotSpeed;
                if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) playerDir += rotSpeed;

                if (moving) {
                    float rhythm = (currentSpeed > moveSpeed) ? 1.5f : 1.0f;
                    walkTimer += 8.0f * 0.016f * rhythm;
                    stepTimer -= 0.016f * rhythm;
                    if (stepTimer <= 0.0f) { playStepSound(); stepTimer = STEP_INTERVAL; }
                }
                else { walkTimer = 0.0f; stepTimer = 0.05f; }

                checkWeaponCollection(playerX, playerY, playerHealth, playerArmor);
                updateSprites(playerX, playerY, playerHealth, playerArmor);
                updateFireballs(playerX, playerY, 0.016f, playerHealth, playerArmor, damageAlpha);
                if (checkCollision(playerX, playerY) || playerHealth <= 0) { gameOver = true; playPlayerDeath(); }
            }
        }

        vertices.clear(); glUseProgram(p);

        if (gameState == PLAYING) {
            if (!gameOver) {
                glUniform1f(glGetUniformLocation(p, "playerDir"), playerDir); glUniform2f(glGetUniformLocation(p, "playerPos"), playerX, playerY);
                glUniform1f(glGetUniformLocation(p, "screenWidth"), (float)screenWidth); glUniform1f(glGetUniformLocation(p, "screenHeight"), (float)screenHeight);

                float cB = 6.0f, fB = 5.0f;
                vertices.insert(vertices.end(), { -1,1,1,1,cB,0,0, 1,1,1,1,cB,0,0, 1,0,1,1,cB,0,0, -1,1,1,1,cB,0,0, 1,0,1,1,cB,0,0, -1,0,1,1,cB,0,0 });
                vertices.insert(vertices.end(), { -1,-1,1,1,fB,0,0, 1,-1,1,1,fB,0,0, 1,0,1,1,fB,0,0, -1,-1,1,1,fB,0,0, 1,0,1,1,fB,0,0, -1,0,1,1,fB,0,0 });

                glActiveTexture(GL_TEXTURE0);
                for (int x = 0; x < screenWidth; x++) {
                    float cX = 2.0f * x / screenWidth - 1.0f, rDX = cos(playerDir) + cX * cos(playerDir + M_PI / 2), rDY = sin(playerDir) + cX * sin(playerDir + M_PI / 2);
                    int mX = (int)playerX, mY = (int)playerY; float dDX = abs(1 / rDX), dDY = abs(1 / rDY), sDX, sDY; int sX, sY, side, hit = 0;
                    if (rDX < 0) { sX = -1; sDX = (playerX - mX) * dDX; }
                    else { sX = 1; sDX = (mX + 1.0f - playerX) * dDX; }
                    if (rDY < 0) { sY = -1; sDY = (playerY - mY) * dDY; }
                    else { sY = 1; sDY = (mY + 1.0f - playerY) * dDY; }
                    float texOffset = 0.0f;
                    while (!hit) {
                        if (sDX < sDY) { sDX += dDX; mX += sX; side = 0; }
                        else { sDY += dDY; mY += sY; side = 1; }
                        int tile = worldMap[mY][mX];
                        if (tile == 1 || tile == 9) hit = 1;
                        else if ((tile >= 2 && tile <= 5) || tile == 8) {
                            float perpD = (side == 0) ? (mX - playerX + (1 - sX) / 2.0f) / rDX : (mY - playerY + (1 - sY) / 2.0f) / rDY;
                            float wX_temp = (side == 0) ? playerY + perpD * rDY : playerX + perpD * rDX;
                            wX_temp -= floor(wX_temp);
                            Door* d = getDoor(mX, mY);
                            if (d) { if (wX_temp > d->openAmount) { hit = 1; texOffset = d->openAmount; } }
                            else hit = 1;
                        }
                    }

                    if (zBuffer.size() != screenWidth) zBuffer.resize(screenWidth);

                    float perp = (side == 0) ? (mX - playerX + (1 - sX) / 2.0f) / rDX : (mY - playerY + (1 - sY) / 2.0f) / rDY; zBuffer[x] = perp;
                    int type = worldMap[mY][mX];
                    float r = (type == 9) ? 1 : (type == 1 ? 0.4 : ((type >= 2 && type <= 5) || type == 8 ? 1.0 : 0.6)), g = r, b = (type == 9) ? 0 : r;
                    if (side) { r *= 0.7; g *= 0.7; b *= 0.7; }
                    float light = 3.5f / (perp + 0.1f); if (light > 1.0f) light = 1.0f; r *= light; g *= light;

                    float wX = (side == 0) ? playerY + perp * rDY : playerX + perp * rDX; wX -= floor(wX);
                    float tX = wX; if ((side == 0 && rDX > 0) || (side == 1 && rDY < 0)) tX = 1 - tX;
                    if ((type >= 2 && type <= 5) || type == 8) { tX -= texOffset; if (tX < 0) tX += 1.0f; }

                    float lH = screenHeight / perp, dS = -lH / 2 + screenHeight / 2, dE = lH / 2 + screenHeight / 2;
                    float nS = 1 - 2 * dS / screenHeight, nE = 1 - 2 * dE / screenHeight, xL = 2.0f * x / screenWidth - 1, xR = 2.0f * (x + 1) / screenWidth - 1;
                    float blueChannelID = 0.0f;
                    if (type == 9) blueChannelID = 31.0f;
                    else if (type == 2) blueChannelID = 36.0f;
                    else if (type == 3) { Door* d = getDoor(mX, mY); blueChannelID = (d && d->isLocked) ? 103.0f : 104.0f; }
                    else if (type == 4) { Door* d = getDoor(mX, mY); blueChannelID = (d && d->isLocked) ? 105.0f : 106.0f; }
                    else if (type == 5) { Door* d = getDoor(mX, mY); blueChannelID = (d && d->isLocked) ? 107.0f : 108.0f; }
                    else if (type == 8) { Door* d = getDoor(mX, mY); blueChannelID = (d && d->isLocked) ? 110.0f : 111.0f; }
                    else blueChannelID = 31.0f;

                    vertices.insert(vertices.end(), { xL,nS,r,g,blueChannelID,tX,1, xR,nS,r,g,blueChannelID,tX,1, xR,nE,r,g,blueChannelID,tX,0, xL,nS,r,g,blueChannelID,tX,1, xR,nE,r,g,blueChannelID,tX,0, xL,nE,r,g,blueChannelID,tX,0 });

                    for (const auto& decal : wallDecals) {
                        if (decal.x == mX && decal.y == mY && decal.side == side) {
                            float decalWidth = 0.25f; float halfWidth = decalWidth / 2.0f;
                            if (tX >= decal.hitX - halfWidth && tX <= decal.hitX + halfWidth) {
                                float dTX = (tX - (decal.hitX - halfWidth)) / decalWidth;
                                float decalHeightPx = lH * decalWidth;
                                float pixelOffset = decal.hitY * lH;
                                float dS_decal = (screenHeight / 2) - (decalHeightPx / 2) - pixelOffset;
                                float dE_decal = (screenHeight / 2) + (decalHeightPx / 2) - pixelOffset;
                                float nS_decal = 1 - 2 * dS_decal / screenHeight; float nE_decal = 1 - 2 * dE_decal / screenHeight;
                                float decalID = (float)decal.type + 0.0f;
                                vertices.insert(vertices.end(), { xL,nS_decal,r,g,decalID,dTX,1, xR,nS_decal,r,g,decalID,dTX,1, xR,nE_decal,r,g,decalID,dTX,0, xL,nS_decal,r,g,decalID,dTX,1, xR,nE_decal,r,g,decalID,dTX,0, xL,nE_decal,r,g,decalID,dTX,0 });
                            }
                        }
                    }
                }

                std::vector<Sprite> toRender;
                for (const auto& s : sprites) if (s.isAlive) toRender.push_back(s);
                for (const auto& bp : bloodParticles) { Sprite s; s.x = bp.x; s.y = bp.y; s.zOffset = bp.z; s.type = OBJECT_BLOOD; s.isWeapon = false; s.dist = (playerX - bp.x) * (playerX - bp.x) + (playerY - bp.y) * (playerY - bp.y); toRender.push_back(s); }
                for (const auto& w : weapons) if (!w.isCollected) { Sprite t; t.x = w.x; t.y = w.y; t.isWeapon = true; t.type = w.type; t.dist = (playerX - w.x) * (playerX - w.x) + (playerY - w.y) * (playerY - w.y); toRender.push_back(t); }
                for (const auto& f : fireballs) { Sprite t; t.x = f.x; t.y = f.y; t.type = 999; t.isWeapon = true; t.dist = (playerX - f.x) * (playerX - f.x) + (playerY - f.y) * (playerY - f.y); toRender.push_back(t); }
                std::sort(toRender.begin(), toRender.end(), [](const Sprite& a, const Sprite& b) {return a.dist > b.dist; });
                float iD = 1.0f / (cos(playerDir + M_PI / 2) * sin(playerDir) - cos(playerDir) * sin(playerDir + M_PI / 2));
                for (const auto& s : toRender) {
                    float sX = s.x - playerX, sY = s.y - playerY;
                    float tX = iD * (sin(playerDir) * sX - cos(playerDir) * sY), tY = iD * (-sin(playerDir + M_PI / 2) * sX + cos(playerDir + M_PI / 2) * sY);
                    if (tY > 0.1f) {
                        int scrX = int(screenWidth / 2 * (1 + tX / tY));
                        float scale = 1.0f;
                        if (s.isWeapon) {
                            if (s.type == 0)scale = 0.3f; if (s.type == 2 || s.type == 3 || s.type == 4 || s.type == 5)scale = 0.4f; else if (s.type == 6 || s.type == 7) scale = 0.4f;
                        }
                        if (s.type == 999) scale = 0.5f; if (s.type == OBJECT_BLOOD) scale = 0.1f; if (s.type == OBJECT_HOLE_PISTOL) scale = 0.15f; if (s.type == OBJECT_HOLE_SHOTGUN) scale = 0.30f;
                        float vOffset = 0.0f; float shadowWidthFactor = 0.0f; float wallH = screenHeight / tY;
                        if (s.type == 999) { float time = (float)glfwGetTime(); vOffset = sin(time * 15.0f + s.x) * 5.0f; shadowWidthFactor = 0.8f; }
                        else if (s.isWeapon) {
                            float time = (float)glfwGetTime();
                            if (s.type == OBJECT_HOLE_PISTOL || s.type == OBJECT_HOLE_SHOTGUN) vOffset = 0.0f;
                            else if (s.type == 6 || s.type == 7 || s.type == 0 || s.type == 1 || s.type == 8) { vOffset = sin(time * 4.0f + s.x) * 10.0f; shadowWidthFactor = 1.0f - (sin(time * 4.0f + s.x) * 0.3f); }
                            else { vOffset = sin(time * 3.0f + s.x) * 3.0f + wallH * 0.6f; shadowWidthFactor = 1.0f - (sin(time * 3.0f + s.x) * 0.3f); }
                        }
                        else if (s.type == OBJECT_BLOOD) vOffset = -s.zOffset * abs(int(screenHeight / tY));
                        else if (!s.isWeapon && s.type == 2) { scale = 0.8f; vOffset = -abs(int(screenHeight / tY)) * 0.4f; }
                        else if (!s.isWeapon && s.type == 3) { scale = 0.5f; int normalH = abs(int(screenHeight / tY)); int sH_calc = abs(int((screenHeight / tY) * scale)); vOffset = (normalH - sH_calc) / 2.0f; }
                        else if (s.type == 999) vOffset = -abs(int(screenHeight / tY)) * 0.3f;

                        if (shadowWidthFactor > 0.0f) {
                            int sH_item = abs(int(screenHeight / tY * scale)); int sW_item = sH_item / 2;
                            int sW_shadow = sW_item * shadowWidthFactor * 1.2f; int sH_shadow = sH_item * 0.15f;
                            float floorStatic = wallH * 0.6f; float shadowVOffset = floorStatic + (sH_item / 2.0f);
                            float ndcS = 1 - 2.0f * (screenHeight / 2 - sH_shadow / 2 + shadowVOffset) / screenHeight;
                            float ndcE = 1 - 2.0f * (screenHeight / 2 + sH_shadow / 2 + shadowVOffset) / screenHeight;
                            int dS = scrX - sW_shadow / 2, dE = scrX + sW_shadow / 2;
                            for (int str = dS; str < dE; str++) {
                                if (str >= 0 && str < screenWidth && tY < zBuffer[str]) {
                                    float texX = (float)(str - dS) / sW_shadow;
                                    float xL = 2.0f * str / screenWidth - 1, xR = 2.0f * (str + 1) / screenWidth - 1;
                                    float sLight = 3.5f / (tY + 0.1f); if (sLight > 1.0f) sLight = 1.0f;
                                    vertices.insert(vertices.end(), { xL,ndcS,sLight,sLight,96.0f,texX,1, xR,ndcS,sLight,sLight,96.0f,texX,1, xR,ndcE,sLight,sLight,96.0f,texX,0, xL,ndcS,sLight,sLight,96.0f,texX,1, xR,ndcE,sLight,sLight,96.0f,texX,0, xL,ndcE,sLight,sLight,96.0f,texX,0 });
                                }
                            }
                        }

                        int sH = abs(int(screenHeight / tY * scale)); int sW = sH / 2;
                        if (!s.isWeapon && s.type == 3) sW = sH / 1.0;
                        if (s.type == OBJECT_HOLE_PISTOL || s.type == OBJECT_HOLE_SHOTGUN || s.type == OBJECT_BLOOD) sW = sH;
                        int dS = scrX - sW / 2, dE = scrX + sW / 2;
                        for (int str = dS; str < dE; str++) {
                            if (str >= 0 && str < screenWidth && tY < zBuffer[str]) {
                                float texX = (float)(str - dS) / sW;
                                float ndcS = 1 - 2.0f * (screenHeight / 2 - sH / 2 + vOffset) / screenHeight;
                                float ndcE = 1 - 2.0f * (screenHeight / 2 + sH / 2 + vOffset) / screenHeight;
                                float xL = 2.0f * str / screenWidth - 1, xR = 2.0f * (str + 1) / screenWidth - 1;
                                float sLight = 3.5f / (tY + 0.1f); if (sLight > 1.0f) sLight = 1.0f;
                                float id = 1.0f;
                                if (s.type == 999) id = 16.0f;
                                else if (s.isWeapon) {
                                    if (s.type == 1)id = 9.0f; else if (s.type == 0)id = 2.0f; else if (s.type == 2)id = 11.0f; else if (s.type == 3)id = 12.0f; else if (s.type == 4)id = 23.0f; else if (s.type == 5)id = 25.0f;
                                    else if (s.type == 6) id = 101.0f; else if (s.type == 7) id = 102.0f; else if (s.type == 8) id = 50.0f; else if (s.type == 9) id = 53.0f;
                                    else if (s.type == OBJECT_HOLE_PISTOL) id = 98.0f; else if (s.type == OBJECT_HOLE_SHOTGUN) id = 99.0f;
                                }
                                else if (s.type == OBJECT_BLOOD) id = 80.0f;
                                else if (s.type == 2) { if (s.state == 2)id = 15.0f; else if (s.state == 1)id = 14.0f; else id = 13.0f; }
                                else if (s.type == 3) {
                                    if (s.state == 2) id = 20.0f;
                                    else if (s.state == 1) { if (s.fightFrame == 0) id = 21.0f; else id = 22.0f; }
                                    else { if (s.walkStep == 0) id = 17.0f; else if (s.walkStep == 1) id = 19.0f; else if (s.walkStep == 2) id = 17.0f; else id = 18.0f; }
                                }
                                vertices.insert(vertices.end(), { xL,ndcS,sLight,sLight,id,texX,1, xR,ndcS,sLight,sLight,id,texX,1, xR,ndcE,sLight,sLight,id,texX,0, xL,ndcS,sLight,sLight,id,texX,1, xR,ndcE,sLight,sLight,id,texX,0, xL,ndcE,sLight,sLight,id,texX,0 });
                            }
                        }
                    }
                }

                if (finalIntensity > 0.01f) {
                    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
                    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7));
                    vertices.clear();

                    drawQuad2D(vertices, 0.0f, 0.0f, 1.0f, 1.0f, 30.0f);

                    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
                    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7));
                    vertices.clear();
                }

                float bobX = cos(walkTimer) * 0.03f; float bobY = abs(sin(walkTimer)) * 0.05f;
                if (walkTimer == 0.0f) { bobX = 0; bobY = 0; }
                float recoilX = 0.0f; float recoilY = 0.0f;
                if (isShooting) { recoilX = ((float)rand() / RAND_MAX - 0.5f) * 0.1f; recoilY = 0.1f; }

                if (currentWeapon == 0) {
                    float gunID = 24.0f;
                    if (isShooting) {
                        float phase = 1.0f - (shootTimer / maxShootTime);
                        if (phase < 0.25f) gunID = 32.0f; else if (phase < 0.50f) gunID = 33.0f; else if (phase < 0.75f) gunID = 34.0f; else gunID = 35.0f;
                        if (gunID == 32.0f) glActiveTexture(GL_TEXTURE0 + 29); else if (gunID == 33.0f) glActiveTexture(GL_TEXTURE0 + 30); else if (gunID == 34.0f) glActiveTexture(GL_TEXTURE0 + 31); else if (gunID == 35.0f) glActiveTexture(GL_TEXTURE0 + 32);
                    }
                    else glActiveTexture(GL_TEXTURE23);
                    drawQuad2D(vertices, 0.06f + bobX + recoilX, -0.5f - bobY + recoilY, 0.3f, 0.4f, gunID);
                }
                else if (currentWeapon == 1) {
                    glActiveTexture(GL_TEXTURE7); float gunID = isShooting ? 26.0f : 7.0f; if (isShooting) glActiveTexture(GL_TEXTURE25);
                    drawQuad2D(vertices, 0.0f + bobX + recoilX, -0.5f - bobY + recoilY, 0.35f, 0.5f, gunID);
                }
                else if (currentWeapon == 2) {
                    glActiveTexture(GL_TEXTURE9); float gunID = isShooting ? 27.0f : 10.0f; if (isShooting) glActiveTexture(GL_TEXTURE26);
                    drawQuad2D(vertices, 0.15f + bobX + recoilX, -0.4f - bobY + recoilY, 0.35f, 0.5f, gunID);
                }
                else if (currentWeapon == 3) {
                    glActiveTexture(GL_TEXTURE0 + 50); float gunID = isShooting ? 52.0f : 51.0f; if (isShooting) glActiveTexture(GL_TEXTURE0 + 51);
                    drawQuad2D(vertices, 0.15f + bobX + recoilX, -0.5f - bobY + recoilY, 0.45f, 0.40f, gunID);
                }

                for (const auto& s : shells) { float cID = (s.type == 1) ? 131.0f : 130.0f; drawRotatedQuad2D(vertices, s.x, s.y, s.scale, s.scale, s.rotation, cID); }

                glActiveTexture(GL_TEXTURE0 + 53); drawQuad2D(vertices, 0.0f, -0.87f, 0.75f, 0.13f, 125.0f);
                glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
                glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7));
                vertices.clear();

                glActiveTexture(GL_TEXTURE3);
                drawText(vertices, -0.60f, -0.84f, 0.04f, "HP"); drawText(vertices, -0.65f, -0.92f, 0.07f, std::to_string(playerHealth));
                if (playerArmor > 0) { drawText(vertices, -0.40f, -0.84f, 0.04f, "ARMOR"); drawText(vertices, -0.415f, -0.92f, 0.07f, std::to_string(playerArmor)); }
                if (currentWeapon == 0) drawText(vertices, 0.28f, -0.85f, 0.04f, "WEAPON: FISTS");
                else if (currentWeapon == 1) { drawText(vertices, 0.28f, -0.85f, 0.04f, "WEAPON: PISTOL"); drawText(vertices, 0.28f, -0.93f, 0.04f, "AMMO: " + std::to_string(ammoPistol)); }
                else if (currentWeapon == 2) { drawText(vertices, 0.28f, -0.85f, 0.04f, "WEAPON: SHOTGUN"); drawText(vertices, 0.28f, -0.93f, 0.04f, "AMMO: " + std::to_string(ammoShotgun)); }
                else if (currentWeapon == 3) { drawText(vertices, 0.28f, -0.85f, 0.04f, "WEAPON: RIFLE"); drawText(vertices, 0.28f, -0.93f, 0.04f, "AMMO: " + std::to_string(ammoRifle)); }

                float centerX = -0.1f; float cardY = -0.87f; float cardSize = 0.06f;
                if (hasGreenKey) { drawQuad2D(vertices, centerX, cardY, cardSize, cardSize, 101.0f); centerX += 0.12f; }
                if (hasRedKey) { drawQuad2D(vertices, centerX, cardY, cardSize, cardSize, 102.0f); }
                glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
                glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7));

                if (isKeypadActive) {
                    if (!isKeypadSuccess) {
                        for (int k = 0; k <= 9; k++) {
                            if (glfwGetKey(window, GLFW_KEY_0 + k) == GLFW_PRESS) {
                                if (!keysPressed[k]) { if (keypadInput.length() < 4) { keypadInput += std::to_string(k); std::cout << "Kod: " << keypadInput << std::endl; playKeypadClick(); } keysPressed[k] = true; }
                            }
                            else keysPressed[k] = false;
                        }
                        static bool backspacePressed = false;
                        if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS) { if (!backspacePressed && keypadInput.length() > 0) { keypadInput.pop_back(); backspacePressed = true; } }
                        else backspacePressed = false;
                        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) { isKeypadActive = false; keypadInput = ""; }
                        static bool enterPressed = false;
                        if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
                            if (!enterPressed) {
                                if (keypadInput == "1234") { isKeypadSuccess = true; keypadSuccessTimer = 1.0f; if (targetKeypadDoor) { targetKeypadDoor->isLocked = false; targetKeypadDoor->state = OPENING; playDoorSound(); } keypadInput = ""; }
                                else { keypadInput = ""; std::cout << "Bledny kod!" << std::endl; playAccessDeniedSound(); }
                                enterPressed = true;
                            }
                        }
                        else enterPressed = false;
                    }
                    else {
                        keypadSuccessTimer -= 0.016f; if (keypadSuccessTimer <= 0.0f) { isKeypadActive = false; isKeypadSuccess = false; targetKeypadDoor = nullptr; }
                    }

                    vertices.clear(); glUseProgram(p);
                    drawQuad2D(vertices, 0.0f, 0.0f, 1.0f, 1.0f, 95.0f);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
                    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7));
                    vertices.clear();

                    int uiTexID = isKeypadSuccess ? 48 : 47;
                    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, t[uiTexID]);
                    drawQuad2D(vertices, 0.0f, 0.0f, 0.4f, 0.6f, 90.0f);
                    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
                    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7));
                    glBindTexture(GL_TEXTURE_2D, t[0]);

                    if (!isKeypadSuccess) {
                        vertices.clear(); glActiveTexture(GL_TEXTURE3);
                        drawText(vertices, -0.15f, -0.29f, 0.1f, keypadInput);
                        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
                        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7));
                    }
                }
            }
            else if (gameOver) {
                glUniform1f(dmgIntLoc, 0.2f);
                vertices.clear(); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, t[27]);
                drawQuad2D(vertices, 0.0f, 0.0f, 1.0f, 1.0f, 30.0f);
                glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
                glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7));
                vertices.clear();
                glBindTexture(GL_TEXTURE_2D, t[28]);
                drawQuad2D(vertices, 0.0f, 0.0f, 0.6f, 0.4f, 31.0f);
                glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
                glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7));
                glBindTexture(GL_TEXTURE_2D, t[0]);
            }
        }
        else if (gameState == MENU) {
            glUniform1i(useTextureLoc, 1);
            glUniform1f(dmgIntLoc, 0.0f);
            renderMenu(vertices, menuCtx);
            glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
            glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7));
        }

        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO); glDeleteBuffers(1, &VBO); glDeleteProgram(p);

    cleanupAudio();

    glfwTerminate();
    return 0;
}