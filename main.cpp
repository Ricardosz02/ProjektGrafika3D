#include "config.h" 
#include "Map.h"     
#include "Goblin.h"  
#include "Weapon.h"   
#include "stb_image.h" 
#include <cmath>
#include <vector>
#include <iostream>
#include <algorithm> 
#include <string>       

#ifndef M_PI
#define M_PI 3.14159265358979323846f 
#endif

float playerX = 2.5f;
float playerY = 2.5f;
float playerDir = 0.0f;
const int screenWidth = 640;
const int screenHeight = 480;

int playerHealth = 100;
int playerArmor = 0;

float moveSpeed = 0.05f;
float rotSpeed = 0.03f;
bool gameOver = false;

float walkTimer = 0.0f;

const float FISTS_DAMAGE = 15.0f;
const float PISTOL_DAMAGE = 25.0f;
const float SHOTGUN_DAMAGE = 100.0f;

struct HitMarker { float x_map, y_map, life; int side; float texX; };
std::vector<HitMarker> hitMarkers;

struct BulletFlash { float x, y, dirX, dirY, life; };
std::vector<BulletFlash> bulletFlashes;

extern int (*worldMap)[MAP_WIDTH];
int activeMapIndex = 1;

void updateSprites(float playerX, float playerY, int& health, int& armor) {
    if (!sprites.empty()) {
        moveMonsters(playerX, playerY, 0.016f, health, armor);
        removeDeadMonsters();
    }
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

// Sloty 0-24
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

uniform bool useTexture; uniform float playerDir; uniform vec2 playerPos; uniform float screenWidth; uniform float screenHeight;

void main() {
    if (useTexture) {
        vec4 texColor;
        if (ourColor.b > 24.9)      texColor = texture(armorTexture, TexCoord);
        else if (ourColor.b > 23.9) texColor = texture(fistsTexture, TexCoord);
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
        else if (ourColor.b > 9.9)  texColor = texture(shotgunViewTexture, TexCoord); 
        else if (ourColor.b > 8.9)  texColor = texture(shotgunTexture, TexCoord);     
        else if (ourColor.b > 7.9)  texColor = vec4(0.2, 0.2, 0.2, 1.0);              
        else if (ourColor.b > 6.9)  texColor = texture(weaponViewTexture, TexCoord);  
        else if (ourColor.b > 5.9) {
            float p = gl_FragCoord.y - (screenHeight / 2.0); if (p < 1.0) p = 1.0; 
            float posZ = 0.5 * screenHeight; float rowDistance = posZ / p;
            float cameraX = (gl_FragCoord.x / screenWidth) * 2.0 - 1.0;
            float rayDirX = cos(playerDir) + cos(playerDir+1.5708)*cameraX;
            float rayDirY = sin(playerDir) + sin(playerDir+1.5708)*cameraX;
            vec2 ceilPos = playerPos + rowDistance * vec2(rayDirX, rayDirY);
            texColor = texture(ceilingTexture, ceilPos) * vec4(0.8, 0.8, 0.8, 1.0);
        } else if (ourColor.b > 4.9) { 
            float p = (screenHeight / 2.0) - gl_FragCoord.y; if (p < 1.0) p = 1.0; 
            float posZ = 0.5 * screenHeight; float rowDistance = posZ / p;
            float cameraX = (gl_FragCoord.x / screenWidth) * 2.0 - 1.0;
            float rayDirX = cos(playerDir) + cos(playerDir+1.5708)*cameraX;
            float rayDirY = sin(playerDir) + sin(playerDir+1.5708)*cameraX;
            vec2 floorPos = playerPos + rowDistance * vec2(rayDirX, rayDirY);
            texColor = texture(floorTexture, floorPos) * vec4(0.8, 0.8, 0.8, 1.0);
        } else if (ourColor.b > 3.9) texColor = texture(hitTexture, TexCoord); 
        else if (ourColor.b > 2.9) texColor = texture(fontTexture, TexCoord);
        else if (ourColor.b > 1.9) texColor = texture(pistolTexture, TexCoord);
        else if (ourColor.b > 0.99) texColor = texture(monsterTexture, TexCoord); 
        else texColor = texture(wallTexture, TexCoord);
        
        if (texColor.a < 0.1) discard; 
        FragColor = texColor * vec4(ourColor.r, ourColor.g, min(ourColor.b, 1.0), 1.0);
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
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) { GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB; glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data); glGenerateMipmap(GL_TEXTURE_2D); }
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

    GLuint p = glCreateProgram(); // p = shaderProgram
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource), fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    glAttachShader(p, vs); glAttachShader(p, fs); glLinkProgram(p); glDeleteShader(vs); glDeleteShader(fs);

    GLuint VAO, VBO; glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * screenWidth * 40 * 7 * 2, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float))); glEnableVertexAttribArray(2);

    GLuint t[25];
    t[0] = loadTexture("wall.png"); t[1] = loadTexture("monster.png"); t[2] = loadTexture("pistol.png");
    t[3] = loadTexture("font.png"); t[4] = loadTexture("hit.png"); t[5] = loadTexture("floor.png");
    t[6] = loadTexture("ceiling.png"); t[7] = loadTexture("pistol_view_128.png"); t[8] = loadTexture("shotgun.png");
    t[9] = loadTexture("shotgun_view_128.png"); t[10] = loadTexture("ammunition_pistol.png"); t[11] = loadTexture("ammunition_shotgun.png");
    t[12] = loadTexture("monster_flying_1.png"); t[13] = loadTexture("monster_flying_2.png"); t[14] = loadTexture("monster_flying_3.png");
    t[15] = loadTexture("fireball.png"); t[16] = loadTexture("monster_walk_1.png"); t[17] = loadTexture("monster_walk_2.png");
    t[18] = loadTexture("monster_walk_3.png"); t[19] = loadTexture("monster_walk_hit_5.png"); t[20] = loadTexture("monster_walk_fight_1.png");
    t[21] = loadTexture("monster_walk_fight_2.png"); t[22] = loadTexture("mecidal.png"); t[23] = loadTexture("hand_both.png");
    t[24] = loadTexture("bulletproof_armor.png");

    glUseProgram(p);
    GLint useTextureLoc = glGetUniformLocation(p, "useTexture");
    glUniform1i(useTextureLoc, 1);

    const char* names[] = { "wallTexture","monsterTexture","pistolTexture","fontTexture","hitTexture","floorTexture","ceilingTexture",
        "weaponViewTexture","shotgunTexture","shotgunViewTexture","ammoPistolTexture","ammoShotgunTexture","fly1Texture","fly2Texture",
        "fly3Texture","fireballTexture","wWalk1Tex","wWalk2Tex","wWalk3Tex","wHitTex","wFight1Tex","wFight2Tex","medkitTexture","fistsTexture", "armorTexture" };
    for (int i = 0; i < 25; i++) glUniform1i(glGetUniformLocation(p, names[i]), i);

    for (int i = 0; i < 25; i++) { glActiveTexture(GL_TEXTURE0 + i); glBindTexture(GL_TEXTURE_2D, t[i]); }
    glActiveTexture(GL_TEXTURE0);

    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
    std::vector<float> zBuffer(screenWidth), vertices;

    switchMap(activeMapIndex); initMonsters(); initWeapons();

    static bool spacePressedLastFrame = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) currentWeapon = 0;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) if (hasPistol) currentWeapon = 1;
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) if (hasShotgun) currentWeapon = 2;

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressedLastFrame) {
            bool canShoot = (currentWeapon == 0) || (currentWeapon == 1 && ammoPistol > 0) || (currentWeapon == 2 && ammoShotgun > 0);
            if (canShoot) {
                if (currentWeapon == 1) ammoPistol--; else if (currentWeapon == 2) ammoShotgun--;
                if (currentWeapon > 0) bulletFlashes.push_back({ playerX + cos(playerDir) * 0.2f, playerY + sin(playerDir) * 0.2f, cos(playerDir), sin(playerDir), 2.0f });

                float rayDX = cos(playerDir), rayDY = sin(playerDir);
                float dist = 1e30f; int hSide = 0, hMX = -1, hMY = -1; float hTX = 0;
                int mX = (int)playerX, mY = (int)playerY; float dDX = abs(1 / rayDX), dDY = abs(1 / rayDY);
                int sX = (rayDX < 0) ? -1 : 1, sY = (rayDY < 0) ? -1 : 1; float sDX = (rayDX < 0) ? (playerX - mX) * dDX : (mX + 1 - playerX) * dDX, sDY = (rayDY < 0) ? (playerY - mY) * dDY : (mY + 1 - playerY) * dDY;
                int hit = 0, side;
                while (!hit) { if (sDX < sDY) { sDX += dDX; mX += sX; side = 0; } else { sDY += dDY; mY += sY; side = 1; } if (worldMap[mY][mX] > 0)hit = 1; }
                dist = (side == 0) ? (mX - playerX + (1 - sX) / 2.0f) / rayDX : (mY - playerY + (1 - sY) / 2.0f) / rayDY;
                if (side == 0)hTX = playerY + dist * rayDY; else hTX = playerX + dist * rayDX; hTX -= floor(hTX);
                if ((side == 0 && rayDX > 0) || (side == 1 && rayDY < 0)) hTX = 1 - hTX;
                hSide = side; hMX = mX; hMY = mY;

                float bestDist = 1e30f; int bestIdx = -1;
                for (int i = 0; i < sprites.size(); ++i) {
                    if (!sprites[i].isAlive || sprites[i].isWeapon) continue;
                    float sX = sprites[i].x - playerX, sY = sprites[i].y - playerY;
                    float iD = 1.0f / (cos(playerDir + M_PI / 2) * sin(playerDir) - cos(playerDir) * sin(playerDir + M_PI / 2));
                    float tY = iD * (-sin(playerDir + M_PI / 2) * sX + cos(playerDir + M_PI / 2) * sY);
                    float tX = iD * (sin(playerDir) * sX - cos(playerDir) * sY);
                    if (tY > 0 && abs(tX / tY) < 0.1f && tY < bestDist) { bestDist = tY; bestIdx = i; }
                }
                float dmg = (currentWeapon == 0) ? FISTS_DAMAGE : (currentWeapon == 1 ? PISTOL_DAMAGE : SHOTGUN_DAMAGE);
                float rng = (currentWeapon == 0) ? 1.0f : 100.0f;
                if (bestIdx != -1 && bestDist < rng && bestDist < dist) hitMonster(bestIdx, dmg);
                else if (currentWeapon > 0 && hit) hitMarkers.push_back({ (float)hMX + 0.5f, (float)hMY + 0.5f, 100.0f, hSide, hTX });
            }
            spacePressedLastFrame = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) spacePressedLastFrame = false;

        if (!gameOver) {
            float mS = moveSpeed; bool moving = false;
            auto tryMove = [&](float moveStep) {
                float nx = playerX + cos(playerDir) * moveStep;
                float ny = playerY + sin(playerDir) * moveStep;
                int type = worldMap[(int)ny][(int)nx];
                if (type == 0) { playerX = nx; playerY = ny; moving = true; }
                else if (type == 9) {
                    if (activeMapIndex == 1) { switchMap(2); activeMapIndex = 2; playerX = 2.5f; playerY = 2.5f; initMonsters(); initWeapons(); }
                    else { switchMap(1); activeMapIndex = 1; playerX = 2.5f; playerY = 7.5f; initMonsters(); initWeapons(); }
                }
                };
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) tryMove(mS);
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) tryMove(-mS);
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) playerDir -= rotSpeed;
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) playerDir += rotSpeed;

            if (moving) walkTimer += 8.0f * 0.016f; else walkTimer = 0.0f;

            checkWeaponCollection(playerX, playerY, playerHealth, playerArmor);
            updateSprites(playerX, playerY, playerHealth, playerArmor);
            updateFireballs(playerX, playerY, 0.016f, playerHealth, playerArmor);
            if (checkCollision(playerX, playerY) || playerHealth <= 0) gameOver = true;
        }

        vertices.clear(); glUseProgram(p);
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
                while (!hit) { if (sDX < sDY) { sDX += dDX; mX += sX; side = 0; } else { sDY += dDY; mY += sY; side = 1; } if (worldMap[mY][mX] > 0)hit = 1; }
                float perp = (side == 0) ? (mX - playerX + (1 - sX) / 2.0f) / rDX : (mY - playerY + (1 - sY) / 2.0f) / rDY; zBuffer[x] = perp;
                int type = worldMap[mY][mX]; float r = (type == 9) ? 1 : (type == 1 ? 0.4 : 0.6), g = r, b = (type == 9) ? 0 : r; if (side)r *= 0.7, g *= 0.7, b *= 0.7;
                float wX = (side == 0) ? playerY + perp * rDY : playerX + perp * rDX; wX -= floor(wX); float tX = wX; if ((side == 0 && rDX > 0) || (side == 1 && rDY < 0))tX = 1 - tX;
                float lH = screenHeight / perp, dS = -lH / 2 + screenHeight / 2, dE = lH / 2 + screenHeight / 2;
                float nS = 1 - 2 * dS / screenHeight, nE = 1 - 2 * dE / screenHeight, xL = 2.0f * x / screenWidth - 1, xR = 2.0f * (x + 1) / screenWidth - 1;
                vertices.insert(vertices.end(), { xL,nS,r,g,b,tX,1, xR,nS,r,g,b,tX,1, xR,nE,r,g,b,tX,0, xL,nS,r,g,b,tX,1, xR,nE,r,g,b,tX,0, xL,nE,r,g,b,tX,0 });
            }

            std::vector<Sprite> toRender;
            for (const auto& s : sprites) if (s.isAlive) toRender.push_back(s);
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
                    if (s.isWeapon) { if (s.type == 0)scale = 0.5f; if (s.type == 2 || s.type == 3 || s.type == 4 || s.type == 5)scale = 0.4f; }
                    if (s.type == 999)scale = 0.5f;

                    // --- OBLICZANIE ODSUWANIA I SKALOWANIA DLA POTWORÓW ---
                    float vOffset = 0.0f;
                    if (!s.isWeapon) {
                        if (s.type == 2) {
                            scale = 0.8f;
                            // <--- ZMIANA: LATANIE (UJEMNY OFFSET) - w górê
                            int normalH = abs(int(screenHeight / tY));
                            vOffset = -normalH * 0.3f; // Podniesiony o 40% wysokoœci œciany
                        }
                        if (s.type == 3) {
                            scale = 0.4f; // Ma³y
                            // Dopychanie do pod³ogi (dodatni offset)
                            int normalH = abs(int(screenHeight / tY));
                            int sH_calc = abs(int((screenHeight / tY) * scale));
                            vOffset = (normalH - sH_calc) / 2.0f;
                        }
                    }

                    int sH = abs(int(screenHeight / tY * scale));
                    int sW = sH / 2;
                    if (!s.isWeapon && s.type == 3) sW = sH / 1.0;

                    int dS = scrX - sW / 2, dE = scrX + sW / 2;
                    for (int str = dS; str < dE; str++) {
                        if (str >= 0 && str < screenWidth && tY < zBuffer[str]) {
                            float texX = (float)(str - dS) / sW;
                            // Aplikujemy vOffset
                            float ndcS = 1 - 2.0f * (screenHeight / 2 - sH / 2 + vOffset) / screenHeight;
                            float ndcE = 1 - 2.0f * (screenHeight / 2 + sH / 2 + vOffset) / screenHeight;

                            float xL = 2.0f * str / screenWidth - 1, xR = 2.0f * (str + 1) / screenWidth - 1;
                            float id = 1.0f;
                            if (s.type == 999) id = 16.0f;
                            else if (s.isWeapon) {
                                if (s.type == 1)id = 9.0f; else if (s.type == 0)id = 2.0f; else if (s.type == 2)id = 11.0f; else if (s.type == 3)id = 12.0f; else if (s.type == 4)id = 23.0f; else if (s.type == 5)id = 25.0f;
                            }
                            else if (s.type == 2) { if (s.state == 2)id = 15.0f; else if (s.state == 1)id = 14.0f; else id = 13.0f; }
                            else if (s.type == 3) {
                                if (s.state == 2) id = 20.0f;
                                else if (s.state == 1) { if (s.fightFrame == 0) id = 21.0f; else id = 22.0f; }
                                else { if (s.walkStep == 0) id = 17.0f; else if (s.walkStep == 1) id = 19.0f; else if (s.walkStep == 2) id = 17.0f; else id = 18.0f; }
                            }
                            vertices.insert(vertices.end(), { xL,ndcS,1,1,id,texX,1, xR,ndcS,1,1,id,texX,1, xR,ndcE,1,1,id,texX,0, xL,ndcS,1,1,id,texX,1, xR,ndcE,1,1,id,texX,0, xL,ndcE,1,1,id,texX,0 });
                        }
                    }
                }
            }

            float bobX = cos(walkTimer) * 0.03f; float bobY = abs(sin(walkTimer)) * 0.05f;
            if (walkTimer == 0.0f) { bobX = 0; bobY = 0; }

            if (currentWeapon == 0) { glActiveTexture(GL_TEXTURE23); drawQuad2D(vertices, 0.0f + bobX, -0.6f - bobY, 0.3f, 0.4f, 24); }
            else if (currentWeapon == 1) { glActiveTexture(GL_TEXTURE7); drawQuad2D(vertices, 0.0f + bobX, -0.7f - bobY, 0.35f, 0.5f, 7); }
            else if (currentWeapon == 2) { glActiveTexture(GL_TEXTURE9); drawQuad2D(vertices, 0.15f + bobX, -0.4f - bobY, 0.35f, 0.5f, 10); }

            float bC = 8.0f; vertices.insert(vertices.end(), { -1,-0.75f,1,1,bC,0,0, 1,-0.75f,1,1,bC,0,0, 1,-1,1,1,bC,0,0, -1,-0.75f,1,1,bC,0,0, 1,-1,1,1,bC,0,0, -1,-1,1,1,bC,0,0 });
            glActiveTexture(GL_TEXTURE3); drawText(vertices, -0.95f, -0.90f, 0.05f, "HP: " + std::to_string(playerHealth) + "%");
            if (playerArmor > 0) drawText(vertices, -0.55f, -0.90f, 0.05f, "ARMOR: " + std::to_string(playerArmor));
            if (currentWeapon == 0) drawText(vertices, -0.25f, -0.90f, 0.05f, "WEAPON: FISTS");
            else if (currentWeapon == 1) { drawText(vertices, -0.25f, -0.90f, 0.05f, "WEAPON: PISTOL"); drawText(vertices, 0.7f, -0.90f, 0.05f, "AMMO: " + std::to_string(ammoPistol)); }
            else if (currentWeapon == 2) { drawText(vertices, -0.25f, -0.90f, 0.05f, "WEAPON: SHOTGUN"); drawText(vertices, 0.7f, -0.90f, 0.05f, "AMMO: " + std::to_string(ammoShotgun)); }
        }
        else {
            glUniform1i(useTextureLoc, 0); drawGameOverText(vertices);
        }

        glClear(GL_COLOR_BUFFER_BIT); glBindBuffer(GL_ARRAY_BUFFER, VBO); glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
        glBindVertexArray(VAO); glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7)); glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO); glDeleteBuffers(1, &VBO); glDeleteProgram(p);
    glfwTerminate();
    return 0;
}