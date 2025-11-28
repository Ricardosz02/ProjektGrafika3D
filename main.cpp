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

const float moveSpeed = 0.05f;
const float rotSpeed = 0.03f;
bool gameOver = false;

const float PISTOL_DAMAGE = 25.0f;
const float SHOTGUN_DAMAGE = 100.0f;

struct HitMarker {
    float x_map = 0.0f, y_map = 0.0f;
    float life = 0.0f;
    int side = 0;
    float texX = 0.0f;
};
std::vector<HitMarker> hitMarkers;

struct BulletFlash {
    float x = 0.0f, y = 0.0f;
    float dirX = 0.0f, dirY = 0.0f;
    float life = 0.0f;
};
std::vector<BulletFlash> bulletFlashes;

extern int (*worldMap)[MAP_WIDTH];
int activeMapIndex = 1;

void updateSprites(float playerX, float playerY, int& health) {
    if (!sprites.empty()) {
        moveMonsters(playerX, playerY, 0.016f, health);
        removeDeadMonsters();
    }
}

const char* vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 TexCoord;                       

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord; 
}
)glsl";

// --- SHADER Z APTECZK¥ ---
const char* fragmentShaderSource = R"glsl(
#version 330 core
in vec3 ourColor;
in vec2 TexCoord;                      
out vec4 FragColor;

// Sloty 0-21
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
// NOWE:
uniform sampler2D medkitTexture;     // 22

uniform bool useTexture;
uniform float playerDir;
uniform vec2 playerPos;
uniform float screenWidth;
uniform float screenHeight;

void main() {
    if (useTexture) {
        vec4 texColor;
        
        // ID > 22.9 -> APTECZKA (Slot 22)
        if (ourColor.b > 22.9)      texColor = texture(medkitTexture, TexCoord);

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
            float dirX = cos(playerDir); float dirY = sin(playerDir);
            float planeX = cos(playerDir + 1.5708); float planeY = sin(playerDir + 1.5708);
            float rayDirX = dirX + planeX * cameraX; float rayDirY = dirY + planeY * cameraX;
            vec2 ceilPos = playerPos + rowDistance * vec2(rayDirX, rayDirY);
            texColor = texture(ceilingTexture, ceilPos);
            texColor = texColor * vec4(0.8, 0.8, 0.8, 1.0);
        } else if (ourColor.b > 4.9) { 
            float p = (screenHeight / 2.0) - gl_FragCoord.y; if (p < 1.0) p = 1.0; 
            float posZ = 0.5 * screenHeight; float rowDistance = posZ / p;
            float cameraX = (gl_FragCoord.x / screenWidth) * 2.0 - 1.0;
            float dirX = cos(playerDir); float dirY = sin(playerDir);
            float planeX = cos(playerDir + 1.5708); float planeY = sin(playerDir + 1.5708);
            float rayDirX = dirX + planeX * cameraX; float rayDirY = dirY + planeY * cameraX;
            vec2 floorPos = playerPos + rowDistance * vec2(rayDirX, rayDirY);
            texColor = texture(floorTexture, floorPos);
            texColor = texColor * vec4(0.8, 0.8, 0.8, 1.0);
        } else if (ourColor.b > 3.9) texColor = texture(hitTexture, TexCoord); 
        else if (ourColor.b > 2.9) texColor = texture(fontTexture, TexCoord);
        else if (ourColor.b > 1.9) texColor = texture(pistolTexture, TexCoord);
        else if (ourColor.b > 0.99) texColor = texture(monsterTexture, TexCoord); 
        else texColor = texture(wallTexture, TexCoord);
        
        if (texColor.a < 0.1) discard; 
        FragColor = texColor * vec4(ourColor.r, ourColor.g, min(ourColor.b, 1.0), 1.0);
    } else {
        FragColor = vec4(ourColor, 1.0);
    }
}
)glsl";


GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cerr << "Shader compile error: " << info << std::endl;
    }
    return shader;
}

GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else { std::cerr << "!!! BLAD LADOWANIA: " << path << std::endl; }
    stbi_image_free(data);
    stbi_set_flip_vertically_on_load(false);
    return textureID;
}

void drawGameOverText(std::vector<float>& vertices) {
    float cx = 0.0f, cy = 0.0f, w = 0.8f, h = 0.2f, r = 1.0f, g = 0.0f, b = 0.0f;
    vertices.insert(vertices.end(), { cx - w, cy + h, r,g,b, 0,0, cx + w, cy + h, r,g,b, 0,0, cx + w, cy - h, r,g,b, 0,0, cx - w, cy + h, r,g,b, 0,0, cx + w, cy - h, r,g,b, 0,0, cx - w, cy - h, r,g,b, 0,0 });
}

void drawQuad2D(std::vector<float>& vertices, float x, float y, float w, float h, float colorB) {
    float x1 = x - w, y1 = y - h, x2 = x + w, y2 = y + h, r = 1.0f, g = 1.0f, b = colorB;
    vertices.insert(vertices.end(), { x1,y2,r,g,b,0,1, x2,y2,r,g,b,1,1, x2,y1,r,g,b,1,0, x1,y2,r,g,b,0,1, x2,y1,r,g,b,1,0, x1,y1,r,g,b,0,0 });
}

void drawChar(std::vector<float>& vertices, float x, float y, float size, char c) {
    if (c < 32 || c > 126) return;
    int col = (c - 32) % 16, row = (c - 32) / 16;
    float cw = 1.0f / 16, ch = 1.0f / 6;
    float u1 = col * cw, v1 = 1.0f - (row + 1) * ch, u2 = (col + 1) * cw, v2 = 1.0f - row * ch;
    float r = 1.0f, g = 1.0f, b = 3.0f, x2 = x + size, y2 = y + size;
    vertices.insert(vertices.end(), { x,y2,r,g,b,u1,v2, x2,y2,r,g,b,u2,v2, x2,y,r,g,b,u2,v1, x,y2,r,g,b,u1,v2, x2,y,r,g,b,u2,v1, x,y,r,g,b,u1,v1 });
}

void drawText(std::vector<float>& vertices, float startX, float startY, float charWidthNDC, const std::string& text) {
    float currentX = startX;
    for (char c : text) { drawChar(vertices, currentX, startY, charWidthNDC, c); currentX += charWidthNDC * 0.65f; }
}

int main() {
    GLFWwindow* window;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(screenWidth, screenHeight, "Mini DOOM - Medkit Added", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint shaderProgram = glCreateProgram();
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource), fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    glAttachShader(shaderProgram, vs); glAttachShader(shaderProgram, fs); glLinkProgram(shaderProgram);
    glDeleteShader(vs); glDeleteShader(fs);

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO); glGenBuffers(1, &VBO);
    glBindVertexArray(VAO); glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * screenWidth * 35 * 7 * 2, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float))); glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float))); glEnableVertexAttribArray(2);

    // --- LADOWANIE ---
    GLuint tWall = loadTexture("wall.png"), tMon = loadTexture("monster.png"), tPist = loadTexture("pistol.png");
    GLuint tFont = loadTexture("font.png"), tHit = loadTexture("hit.png"), tFloor = loadTexture("floor.png");
    GLuint tCeil = loadTexture("ceiling.png"), tPView = loadTexture("pistol_view_128.png");
    GLuint tShot = loadTexture("shotgun.png"), tSView = loadTexture("shotgun_view_128.png");
    GLuint tAPist = loadTexture("ammunition_pistol.png"), tAShot = loadTexture("ammunition_shotgun.png");
    GLuint tFly1 = loadTexture("monster_flying_1.png"), tFly2 = loadTexture("monster_flying_2.png"), tFly3 = loadTexture("monster_flying_3.png");
    GLuint tFire = loadTexture("fireball.png");
    GLuint tW1 = loadTexture("monster_walk_1.png");
    GLuint tW2 = loadTexture("monster_walk_2.png");
    GLuint tW3 = loadTexture("monster_walk_3.png");
    GLuint tWHit = loadTexture("monster_walk_hit_5.png");
    GLuint tWF1 = loadTexture("monster_walk_fight_1.png");
    GLuint tWF2 = loadTexture("monster_walk_fight_2.png");

    // APTECZKA
    GLuint tMed = loadTexture("mecidal.png"); // Slot 22

    GLint useTextureLoc = glGetUniformLocation(shaderProgram, "useTexture");
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "wallTexture"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "monsterTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "pistolTexture"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "fontTexture"), 3);
    glUniform1i(glGetUniformLocation(shaderProgram, "hitTexture"), 4);
    glUniform1i(glGetUniformLocation(shaderProgram, "floorTexture"), 5);
    glUniform1i(glGetUniformLocation(shaderProgram, "ceilingTexture"), 6);
    glUniform1i(glGetUniformLocation(shaderProgram, "weaponViewTexture"), 7);
    glUniform1i(glGetUniformLocation(shaderProgram, "shotgunTexture"), 8);
    glUniform1i(glGetUniformLocation(shaderProgram, "shotgunViewTexture"), 9);
    glUniform1i(glGetUniformLocation(shaderProgram, "ammoPistolTexture"), 10);
    glUniform1i(glGetUniformLocation(shaderProgram, "ammoShotgunTexture"), 11);
    glUniform1i(glGetUniformLocation(shaderProgram, "fly1Texture"), 12);
    glUniform1i(glGetUniformLocation(shaderProgram, "fly2Texture"), 13);
    glUniform1i(glGetUniformLocation(shaderProgram, "fly3Texture"), 14);
    glUniform1i(glGetUniformLocation(shaderProgram, "fireballTexture"), 15);
    glUniform1i(glGetUniformLocation(shaderProgram, "wWalk1Tex"), 16);
    glUniform1i(glGetUniformLocation(shaderProgram, "wWalk2Tex"), 17);
    glUniform1i(glGetUniformLocation(shaderProgram, "wWalk3Tex"), 18);
    glUniform1i(glGetUniformLocation(shaderProgram, "wHitTex"), 19);
    glUniform1i(glGetUniformLocation(shaderProgram, "wFight1Tex"), 20);
    glUniform1i(glGetUniformLocation(shaderProgram, "wFight2Tex"), 21);
    // Nowy slot
    glUniform1i(glGetUniformLocation(shaderProgram, "medkitTexture"), 22);

    GLint playerDirLoc = glGetUniformLocation(shaderProgram, "playerDir"), playerPosLoc = glGetUniformLocation(shaderProgram, "playerPos");
    GLint screenWLoc = glGetUniformLocation(shaderProgram, "screenWidth"), screenHLoc = glGetUniformLocation(shaderProgram, "screenHeight");

    // Aktywacja
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, tWall);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, tMon);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, tPist);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, tFont);
    glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, tHit);
    glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, tFloor);
    glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, tCeil);
    glActiveTexture(GL_TEXTURE7); glBindTexture(GL_TEXTURE_2D, tPView);
    glActiveTexture(GL_TEXTURE8); glBindTexture(GL_TEXTURE_2D, tShot);
    glActiveTexture(GL_TEXTURE9); glBindTexture(GL_TEXTURE_2D, tSView);
    glActiveTexture(GL_TEXTURE10); glBindTexture(GL_TEXTURE_2D, tAPist);
    glActiveTexture(GL_TEXTURE11); glBindTexture(GL_TEXTURE_2D, tAShot);
    glActiveTexture(GL_TEXTURE12); glBindTexture(GL_TEXTURE_2D, tFly1);
    glActiveTexture(GL_TEXTURE13); glBindTexture(GL_TEXTURE_2D, tFly2);
    glActiveTexture(GL_TEXTURE14); glBindTexture(GL_TEXTURE_2D, tFly3);
    glActiveTexture(GL_TEXTURE15); glBindTexture(GL_TEXTURE_2D, tFire);
    glActiveTexture(GL_TEXTURE16); glBindTexture(GL_TEXTURE_2D, tW1);
    glActiveTexture(GL_TEXTURE17); glBindTexture(GL_TEXTURE_2D, tW2);
    glActiveTexture(GL_TEXTURE18); glBindTexture(GL_TEXTURE_2D, tW3);
    glActiveTexture(GL_TEXTURE19); glBindTexture(GL_TEXTURE_2D, tWHit);
    glActiveTexture(GL_TEXTURE20); glBindTexture(GL_TEXTURE_2D, tWF1);
    glActiveTexture(GL_TEXTURE21); glBindTexture(GL_TEXTURE_2D, tWF2);
    glActiveTexture(GL_TEXTURE22); glBindTexture(GL_TEXTURE_2D, tMed); // Aktywacja apteczki
    glActiveTexture(GL_TEXTURE0);

    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);
    std::vector<float> zBuffer(screenWidth);
    std::vector<float> vertices;

    switchMap(activeMapIndex);
    initMonsters();
    initWeapons();

    static bool spacePressedLastFrame = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) if (hasPistol) currentWeapon = 1;
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) if (hasShotgun) currentWeapon = 2;

        if (currentWeapon > 0 && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressedLastFrame) {
            bool canShoot = (currentWeapon == 1 && ammoPistol > 0) || (currentWeapon == 2 && ammoShotgun > 0);
            if (canShoot) {
                if (currentWeapon == 1) ammoPistol--; else ammoShotgun--;
                float rayDirX = cos(playerDir), rayDirY = sin(playerDir);
                bulletFlashes.push_back({ playerX + rayDirX * 0.2f, playerY + rayDirY * 0.2f, rayDirX, rayDirY, 2.0f });

                float perpWallDist = 1e30f; int hitSide = 0, hitMapX = -1, hitMapY = -1; float hitTexX = 0;
                int mapX = (int)playerX, mapY = (int)playerY;
                float dDX = abs(1.0f / rayDirX), dDY = abs(1.0f / rayDirY);
                int sX, sY; float sDX, sDY;
                if (rayDirX < 0) { sX = -1; sDX = (playerX - mapX) * dDX; }
                else { sX = 1; sDX = (mapX + 1.0f - playerX) * dDX; }
                if (rayDirY < 0) { sY = -1; sDY = (playerY - mapY) * dDY; }
                else { sY = 1; sDY = (mapY + 1.0f - playerY) * dDY; }
                int hit = 0, side;
                while (!hit) {
                    if (sDX < sDY) { sDX += dDX; mapX += sX; side = 0; }
                    else { sDY += dDY; mapY += sY; side = 1; }
                    if (worldMap[mapY][mapX] > 0) hit = 1;
                }
                perpWallDist = (side == 0) ? (mapX - playerX + (1 - sX) / 2.0f) / rayDirX : (mapY - playerY + (1 - sY) / 2.0f) / rayDirY;
                if (side == 0)hitTexX = playerY + perpWallDist * rayDirY; else hitTexX = playerX + perpWallDist * rayDirX;
                hitTexX -= floor(hitTexX); if ((side == 0 && rayDirX > 0) || (side == 1 && rayDirY < 0)) hitTexX = 1.0f - hitTexX;
                hitSide = side; hitMapX = mapX; hitMapY = mapY;

                float bestMonsterDist = 1e30f; int bestMonsterIndex = -1;
                for (int i = 0; i < sprites.size(); ++i) {
                    if (!sprites[i].isAlive || sprites[i].isWeapon) continue;
                    float sX = sprites[i].x - playerX, sY = sprites[i].y - playerY;
                    float invDet = 1.0f / (cos(playerDir + M_PI / 2) * sin(playerDir) - cos(playerDir) * sin(playerDir + M_PI / 2));
                    float tY = invDet * (-sin(playerDir + M_PI / 2) * sX + cos(playerDir + M_PI / 2) * sY);
                    float tX = invDet * (sin(playerDir) * sX - cos(playerDir) * sY);
                    if (tY > 0 && abs(tX / tY) < 0.1f && tY < bestMonsterDist) { bestMonsterDist = tY; bestMonsterIndex = i; }
                }
                float damage = (currentWeapon == 2) ? SHOTGUN_DAMAGE : PISTOL_DAMAGE;
                if (bestMonsterIndex != -1 && bestMonsterDist < perpWallDist) hitMonster(bestMonsterIndex, damage);
                else hitMarkers.push_back({ (float)hitMapX + 0.5f, (float)hitMapY + 0.5f, 100.0f, hitSide, hitTexX });
            }
            spacePressedLastFrame = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) spacePressedLastFrame = false;

        if (!gameOver) {
            float mS = moveSpeed;
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { float nx = playerX + cos(playerDir) * mS, ny = playerY + sin(playerDir) * mS; if (worldMap[(int)ny][(int)nx] == 0) { playerX = nx; playerY = ny; } }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { float nx = playerX - cos(playerDir) * mS, ny = playerY - sin(playerDir) * mS; if (worldMap[(int)ny][(int)nx] == 0) { playerX = nx; playerY = ny; } }
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) playerDir -= rotSpeed;
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) playerDir += rotSpeed;
            checkWeaponCollection(playerX, playerY, playerHealth); // PRZEKAZUJEMY ZDROWIE!
            updateSprites(playerX, playerY, playerHealth);
            updateFireballs(playerX, playerY, 0.016f, playerHealth);
            if (checkCollision(playerX, playerY) || playerHealth <= 0) gameOver = true;
        }

        vertices.clear();
        glUseProgram(shaderProgram);
        if (!gameOver) {
            glUniform1i(useTextureLoc, 1);
            glUniform1f(playerDirLoc, playerDir); glUniform2f(playerPosLoc, playerX, playerY);
            glUniform1f(screenWLoc, (float)screenWidth); glUniform1f(screenHLoc, (float)screenHeight);

            float cB = 6.0f, fB = 5.0f; // Sufit/Podloga
            vertices.insert(vertices.end(), { -1,1,1,1,cB,0,0, 1,1,1,1,cB,0,0, 1,0,1,1,cB,0,0, -1,1,1,1,cB,0,0, 1,0,1,1,cB,0,0, -1,0,1,1,cB,0,0 });
            vertices.insert(vertices.end(), { -1,-1,1,1,fB,0,0, 1,-1,1,1,fB,0,0, 1,0,1,1,fB,0,0, -1,-1,1,1,fB,0,0, 1,0,1,1,fB,0,0, -1,0,1,1,fB,0,0 });

            // Raycasting
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

            // Sprite Rendering
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
                    if (s.isWeapon) {
                        if (s.type == 0)scale = 0.5f; // Pistolet
                        if (s.type == 2 || s.type == 3 || s.type == 4)scale = 0.4f; // Amunicja i Apteczka
                    }
                    if (s.type == 999)scale = 0.5f; // Fireball

                    int sH = abs(int(screenHeight / tY * scale)), sW = sH / 2;
                    int dS = scrX - sW / 2, dE = scrX + sW / 2;
                    for (int str = dS; str < dE; str++) {
                        if (str >= 0 && str < screenWidth && tY < zBuffer[str]) {
                            float texX = (float)(str - dS) / sW, ndcS = 1 - 2.0f * (screenHeight / 2 - sH / 2) / screenHeight, ndcE = 1 - 2.0f * (screenHeight / 2 + sH / 2) / screenHeight;
                            float xL = 2.0f * str / screenWidth - 1, xR = 2.0f * (str + 1) / screenWidth - 1;
                            float id = 1.0f; // Default Goblin

                            // LOGIKA ID TEKSTURY
                            if (s.type == 999) id = 16.0f; // Fireball
                            else if (s.isWeapon) {
                                if (s.type == 1)id = 9.0f;
                                else if (s.type == 0)id = 2.0f;
                                else if (s.type == 2)id = 11.0f;
                                else if (s.type == 3)id = 12.0f;
                                else if (s.type == 4)id = 23.0f; // ID 23.0 - Apteczka
                            }
                            else if (s.type == 2) { // Flying
                                if (s.state == 2)id = 15.0f; else if (s.state == 1)id = 14.0f; else id = 13.0f;
                            }
                            else if (s.type == 3) { // WALKING MONSTER
                                if (s.state == 2) id = 20.0f; // PAIN
                                else if (s.state == 1) { // ATTACK
                                    if (s.fightFrame == 0) id = 21.0f; else id = 22.0f;
                                }
                                else { // WALK
                                    if (s.walkStep == 0) id = 17.0f; else if (s.walkStep == 1) id = 19.0f;
                                    else if (s.walkStep == 2) id = 17.0f; else id = 18.0f;
                                }
                            }

                            vertices.insert(vertices.end(), { xL,ndcS,1,1,id,texX,1, xR,ndcS,1,1,id,texX,1, xR,ndcE,1,1,id,texX,0, xL,ndcS,1,1,id,texX,1, xR,ndcE,1,1,id,texX,0, xL,ndcE,1,1,id,texX,0 });
                        }
                    }
                }
            }

            // HUD
            if (currentWeapon == 1) { glActiveTexture(GL_TEXTURE7); drawQuad2D(vertices, 0, -0.7f, 0.35f, 0.5f, 7); }
            else if (currentWeapon == 2) { glActiveTexture(GL_TEXTURE9); drawQuad2D(vertices, 0.15f, -0.4f, 0.35f, 0.5f, 10); }
            float bC = 8.0f; vertices.insert(vertices.end(), { -1,-0.75f,1,1,bC,0,0, 1,-0.75f,1,1,bC,0,0, 1,-1,1,1,bC,0,0, -1,-0.75f,1,1,bC,0,0, 1,-1,1,1,bC,0,0, -1,-1,1,1,bC,0,0 });
            glActiveTexture(GL_TEXTURE3);
            drawText(vertices, -0.95f, -0.90f, 0.05f, "HP: " + std::to_string(playerHealth) + "%");
            if (currentWeapon == 1) { drawText(vertices, -0.25f, -0.90f, 0.05f, "WEAPON: PISTOL"); drawText(vertices, 0.7f, -0.90f, 0.05f, "AMMO: " + std::to_string(ammoPistol)); }
            else if (currentWeapon == 2) { drawText(vertices, -0.25f, -0.90f, 0.05f, "WEAPON: SHOTGUN"); drawText(vertices, 0.7f, -0.90f, 0.05f, "AMMO: " + std::to_string(ammoShotgun)); }
            else drawText(vertices, -0.25f, -0.90f, 0.05f, "WEAPON: KNIFE");
        }
        else {
            glUniform1i(useTextureLoc, 0); drawGameOverText(vertices);
        }

        glClear(GL_COLOR_BUFFER_BIT);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(vertices.size() / 7));
        glfwSwapBuffers(window);
    }
    glDeleteVertexArrays(1, &VAO); glDeleteBuffers(1, &VBO); glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}