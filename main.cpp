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

extern void updateSprites(float playerX, float playerY);

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

const char* fragmentShaderSource = R"glsl(
#version 330 core
in vec3 ourColor;
in vec2 TexCoord;                      
out vec4 FragColor;

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

uniform bool useTexture;
uniform float playerDir;
uniform vec2 playerPos;
uniform float screenWidth;
uniform float screenHeight;

void main() {
    if (useTexture) {
        vec4 texColor;
        
        if (ourColor.b > 9.9) {
            texColor = texture(shotgunViewTexture, TexCoord); 
        } else if (ourColor.b > 8.9) {
            texColor = texture(shotgunTexture, TexCoord);     
        } else if (ourColor.b > 7.9) {
            texColor = vec4(0.2, 0.2, 0.2, 1.0);              
        } else if (ourColor.b > 6.9) {
            texColor = texture(weaponViewTexture, TexCoord);  
        } else if (ourColor.b > 5.9) {
            float p = gl_FragCoord.y - (screenHeight / 2.0);
            if (p < 1.0) p = 1.0; 
            float posZ = 0.5 * screenHeight;
            float rowDistance = posZ / p;
            float cameraX = (gl_FragCoord.x / screenWidth) * 2.0 - 1.0;
            float dirX = cos(playerDir); float dirY = sin(playerDir);
            float planeX = cos(playerDir + 1.5708); float planeY = sin(playerDir + 1.5708);
            float rayDirX = dirX + planeX * cameraX; float rayDirY = dirY + planeY * cameraX;
            vec2 ceilPos = playerPos + rowDistance * vec2(rayDirX, rayDirY);
            texColor = texture(ceilingTexture, ceilPos);
            texColor = texColor * vec4(0.8, 0.8, 0.8, 1.0);
        } else if (ourColor.b > 4.9) { 
            float p = (screenHeight / 2.0) - gl_FragCoord.y;
            if (p < 1.0) p = 1.0; 
            float posZ = 0.5 * screenHeight;
            float rowDistance = posZ / p;
            float cameraX = (gl_FragCoord.x / screenWidth) * 2.0 - 1.0;
            float dirX = cos(playerDir); float dirY = sin(playerDir);
            float planeX = cos(playerDir + 1.5708); float planeY = sin(playerDir + 1.5708);
            float rayDirX = dirX + planeX * cameraX; float rayDirY = dirY + planeY * cameraX;
            vec2 floorPos = playerPos + rowDistance * vec2(rayDirX, rayDirY);
            texColor = texture(floorTexture, floorPos);
            texColor = texColor * vec4(0.8, 0.8, 0.8, 1.0);
        } else if (ourColor.b > 3.9) {
            texColor = texture(hitTexture, TexCoord); 
        } else if (ourColor.b > 2.9) {
            texColor = texture(fontTexture, TexCoord);
        } else if (ourColor.b > 1.9) {
            texColor = texture(pistolTexture, TexCoord);
        } else if (ourColor.b > 0.99) {
            texColor = texture(monsterTexture, TexCoord); 
        } else {
            texColor = texture(wallTexture, TexCoord);
        }
        
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
        std::cout << "Zaladowano: " << path << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "!!! BLAD LADOWANIA: " << path << std::endl;
    }
    stbi_image_free(data);
    stbi_set_flip_vertically_on_load(false);
    return textureID;
}

void drawGameOverText(std::vector<float>& vertices) {
    float cx = 0.0f; float cy = 0.0f;
    float w = 0.8f; float h = 0.2f;
    float r = 1.0f; float g = 0.0f; float b = 0.0f;
    vertices.insert(vertices.end(), { cx - w, cy + h, r, g, b, 0.0f, 0.0f });
    vertices.insert(vertices.end(), { cx + w, cy + h, r, g, b, 0.0f, 0.0f });
    vertices.insert(vertices.end(), { cx + w, cy - h, r, g, b, 0.0f, 0.0f });
    vertices.insert(vertices.end(), { cx - w, cy + h, r, g, b, 0.0f, 0.0f });
    vertices.insert(vertices.end(), { cx + w, cy - h, r, g, b, 0.0f, 0.0f });
    vertices.insert(vertices.end(), { cx - w, cy - h, r, g, b, 0.0f, 0.0f });
}

void drawQuad2D(std::vector<float>& vertices, float x, float y, float w, float h, float colorB) {
    float x1 = x - w; float y1 = y - h; float x2 = x + w; float y2 = y + h;
    float r = 1.0f; float g = 1.0f; float b = colorB;
    vertices.insert(vertices.end(), { x1, y2, r, g, b, 0.0f, 1.0f });
    vertices.insert(vertices.end(), { x2, y2, r, g, b, 1.0f, 1.0f });
    vertices.insert(vertices.end(), { x2, y1, r, g, b, 1.0f, 0.0f });
    vertices.insert(vertices.end(), { x1, y2, r, g, b, 0.0f, 1.0f });
    vertices.insert(vertices.end(), { x2, y1, r, g, b, 1.0f, 0.0f });
    vertices.insert(vertices.end(), { x1, y1, r, g, b, 0.0f, 0.0f });
}

void drawChar(std::vector<float>& vertices, float x, float y, float size, char c) {
    if (c < 32 || c > 126) return;
    int atlasCols = 16; int atlasRows = 6; int charIndex = c - 32;
    int row = charIndex / atlasCols; int col = charIndex % atlasCols;
    float charWidth = 1.0f / atlasCols; float charHeight = 1.0f / atlasRows;
    float u1 = col * charWidth; float v1 = 1.0f - (row + 1) * charHeight;
    float u2 = (col + 1) * charWidth; float v2 = 1.0f - row * charHeight;
    float r = 1.0f, g = 1.0f, b = 3.0f;
    float x1 = x; float y1 = y; float x2 = x + size; float y2 = y + size;
    vertices.insert(vertices.end(), { x1, y2, r, g, b, u1, v2 });
    vertices.insert(vertices.end(), { x2, y2, r, g, b, u2, v2 });
    vertices.insert(vertices.end(), { x2, y1, r, g, b, u2, v1 });
    vertices.insert(vertices.end(), { x1, y2, r, g, b, u1, v2 });
    vertices.insert(vertices.end(), { x2, y1, r, g, b, u2, v1 });
    vertices.insert(vertices.end(), { x1, y1, r, g, b, u1, v1 });
}

void drawText(std::vector<float>& vertices, float startX, float startY, float charWidthNDC, const std::string& text) {
    float currentX = startX;
    float spacingStep = charWidthNDC * 0.65f;
    for (char c : text) {
        drawChar(vertices, currentX, startY, charWidthNDC, c);
        currentX += spacingStep;
    }
}


int main() {
    GLFWwindow* window;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screenWidth, screenHeight, "Mini DOOM - Ammo Separation", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * screenWidth * 20 * 7 * 2, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLuint wallTextureID = loadTexture("wall.png");
    GLuint monsterTextureID = loadTexture("monster.png");
    GLuint pistolTextureID = loadTexture("pistol.png");
    GLuint fontTextureID = loadTexture("font.png");
    GLuint hitTextureID = loadTexture("hit.png");
    GLuint floorTextureID = loadTexture("floor.png");
    GLuint ceilingTextureID = loadTexture("ceiling.png");
    GLuint weaponViewTextureID = loadTexture("pistol_view_128.png");
    GLuint shotgunTextureID = loadTexture("shotgun.png");
    GLuint shotgunViewTextureID = loadTexture("shotgun_view_128.png");

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

    GLint playerDirLoc = glGetUniformLocation(shaderProgram, "playerDir");
    GLint playerPosLoc = glGetUniformLocation(shaderProgram, "playerPos");
    GLint screenWLoc = glGetUniformLocation(shaderProgram, "screenWidth");
    GLint screenHLoc = glGetUniformLocation(shaderProgram, "screenHeight");

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, wallTextureID);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, monsterTextureID);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, pistolTextureID);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, hitTextureID);
    glActiveTexture(GL_TEXTURE5); glBindTexture(GL_TEXTURE_2D, floorTextureID);
    glActiveTexture(GL_TEXTURE6); glBindTexture(GL_TEXTURE_2D, ceilingTextureID);
    glActiveTexture(GL_TEXTURE7); glBindTexture(GL_TEXTURE_2D, weaponViewTextureID);
    glActiveTexture(GL_TEXTURE8); glBindTexture(GL_TEXTURE_2D, shotgunTextureID);
    glActiveTexture(GL_TEXTURE9); glBindTexture(GL_TEXTURE_2D, shotgunViewTextureID);
    glActiveTexture(GL_TEXTURE0);

    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

    std::vector<float> zBuffer(screenWidth);
    std::vector<float> vertices;

    std::cout << "System start." << std::endl;
    switchMap(activeMapIndex);
    initGoblin(10.5f, 10.5f);
    initWeapons();

    static bool spacePressedLastFrame = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) { if (hasPistol) currentWeapon = 1; }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) { if (hasShotgun) currentWeapon = 2; }

        if (currentWeapon > 0 && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressedLastFrame) {

            // --- LOGIKA STRZELANIA Z KONKRETNEJ PULI AMUNICJI ---
            bool canShoot = false;
            if (currentWeapon == 1 && ammoPistol > 0) {
                ammoPistol--;
                canShoot = true;
            }
            else if (currentWeapon == 2 && ammoShotgun > 0) {
                ammoShotgun--;
                canShoot = true;
            }

            if (canShoot) {
                float rayDirX = std::cos(playerDir); float rayDirY = std::sin(playerDir);
                BulletFlash flash; flash.x = playerX + rayDirX * 0.2f; flash.y = playerY + rayDirY * 0.2f; flash.dirX = rayDirX; flash.dirY = rayDirY; flash.life = 2.0f;
                bulletFlashes.push_back(flash);

                float perpWallDist = 1e30f; int hitSide = 0; int hitMapX = -1, hitMapY = -1; float hitTexX = 0.0f;
                int mapX = int(playerX); int mapY = int(playerY);
                float deltaDistX = std::abs(1.0f / rayDirX), deltaDistY = std::abs(1.0f / rayDirY);
                int stepX, stepY; float sideDistX, sideDistY;
                if (rayDirX < 0) { stepX = -1; sideDistX = (playerX - mapX) * deltaDistX; }
                else { stepX = 1; sideDistX = (mapX + 1.0f - playerX) * deltaDistX; }
                if (rayDirY < 0) { stepY = -1; sideDistY = (playerY - mapY) * deltaDistY; }
                else { stepY = 1; sideDistY = (mapY + 1.0f - playerY) * deltaDistY; }
                int hit = 0, side;
                while (hit == 0) {
                    if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; }
                    else { sideDistY += deltaDistY; mapY += stepY; side = 1; }
                    if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT && worldMap[mapY][mapX]>0) hit = 1;
                }
                if (hit) {
                    perpWallDist = (side == 0) ? (mapX - playerX + (1.0f - stepX) / 2.0f) / rayDirX : (mapY - playerY + (1.0f - stepY) / 2.0f) / rayDirY;
                    float hitX = playerX + perpWallDist * rayDirX; float hitY = playerY + perpWallDist * rayDirY;
                    float wallX = (side == 0) ? hitY - std::floor(hitY) : hitX - std::floor(hitX);
                    hitTexX = wallX; if (side == 0 && rayDirX > 0) hitTexX = 1.0f - hitTexX; if (side == 1 && rayDirY < 0) hitTexX = 1.0f - hitTexX;
                    hitSide = side; hitMapX = mapX; hitMapY = mapY;
                }

                float perpGoblinDist = 1e30f; bool goblinHit = false;
                if (!sprites.empty() && sprites[0].isAlive) {
                    float spriteX = sprites[0].x - playerX; float spriteY = sprites[0].y - playerY;
                    float invDet = 1.0f / (std::cos(playerDir + M_PI / 2) * std::sin(playerDir) - std::cos(playerDir) * std::sin(playerDir + M_PI / 2));
                    float transformX = invDet * (std::sin(playerDir) * spriteX - std::cos(playerDir) * spriteY);
                    float transformY = invDet * (-std::sin(playerDir + M_PI / 2) * spriteX + std::cos(playerDir + M_PI / 2) * spriteY);
                    if (transformY > 0.0f && std::abs(transformX / transformY) < 0.1f) {
                        perpGoblinDist = transformY; goblinHit = true;
                    }
                }

                float damage = (currentWeapon == 2) ? SHOTGUN_DAMAGE : PISTOL_DAMAGE;
                if (goblinHit && perpGoblinDist < perpWallDist) { hitGoblin(damage); }
                else if (hit) {
                    HitMarker marker; marker.x_map = (float)hitMapX + 0.5f; marker.y_map = (float)hitMapY + 0.5f; marker.life = 100.0f; marker.side = hitSide; marker.texX = hitTexX;
                    hitMarkers.push_back(marker);
                }
            }
            else {
                std::cout << "Klik! Brak amunicji!" << std::endl;
            }
            spacePressedLastFrame = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressedLastFrame = false;
        }

        if (!gameOver) {
            float moveStep = moveSpeed;
            auto checkMove = [&](float dx, float dy) {
                float nx = playerX + dx; float ny = playerY + dy;
                int nextMapX = int(nx); int nextMapY = int(ny);
                if (nextMapY >= 0 && nextMapY < MAP_HEIGHT && nextMapX >= 0 && nextMapX < MAP_WIDTH) {
                    int targetBlock = worldMap[nextMapY][nextMapX];
                    if (targetBlock == 0) { playerX = nx; playerY = ny; }
                    else if (targetBlock == 9) {
                        if (activeMapIndex == 1) { switchMap(2); activeMapIndex = 2; playerX = 2.5f; playerY = 2.5f; initGoblin(5.5f, 5.5f); initWeapons(); }
                        else if (activeMapIndex == 2) { switchMap(1); activeMapIndex = 1; playerX = 2.5f; playerY = 7.5f; initGoblin(10.5f, 10.5f); initWeapons(); }
                    }
                }
                };
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { checkMove(std::cos(playerDir) * moveStep, std::sin(playerDir) * moveStep); }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { checkMove(-std::cos(playerDir) * moveStep, -std::sin(playerDir) * moveStep); }
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { playerDir -= rotSpeed; }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { playerDir += rotSpeed; }

            checkWeaponCollection(playerX, playerY);
            updateSprites(playerX, playerY);

            if (checkCollision(playerX, playerY)) { gameOver = true; std::cout << "GAME OVER" << std::endl; }
        }

        vertices.clear();
        glUseProgram(shaderProgram);

        if (!gameOver) {
            glUniform1i(useTextureLoc, 1);
            glUniform1f(playerDirLoc, playerDir); glUniform2f(playerPosLoc, playerX, playerY);
            glUniform1f(screenWLoc, (float)screenWidth); glUniform1f(screenHLoc, (float)screenHeight);

            float ceilB = 6.0f, floorB = 5.0f;
            vertices.insert(vertices.end(), { -1.0f, 1.0f,1.0f,1.0f,ceilB,0.0f,0.0f, 1.0f,1.0f,1.0f,1.0f,ceilB,0.0f,0.0f, 1.0f,0.0f,1.0f,1.0f,ceilB,0.0f,0.0f });
            vertices.insert(vertices.end(), { -1.0f, 1.0f,1.0f,1.0f,ceilB,0.0f,0.0f, 1.0f,0.0f,1.0f,1.0f,ceilB,0.0f,0.0f, -1.0f,0.0f,1.0f,1.0f,ceilB,0.0f,0.0f });
            vertices.insert(vertices.end(), { -1.0f,-1.0f,1.0f,1.0f,floorB,0.0f,0.0f, 1.0f,-1.0f,1.0f,1.0f,floorB,0.0f,0.0f, 1.0f,0.0f,1.0f,1.0f,floorB,0.0f,0.0f });
            vertices.insert(vertices.end(), { -1.0f,-1.0f,1.0f,1.0f,floorB,0.0f,0.0f, 1.0f,0.0f,1.0f,1.0f,floorB,0.0f,0.0f, -1.0f,0.0f,1.0f,1.0f,floorB,0.0f,0.0f });

            glActiveTexture(GL_TEXTURE0);
            for (int x = 0; x < screenWidth; x++) {
                float cameraX = 2.0f * x / screenWidth - 1.0f;
                float rayDirX = std::cos(playerDir) + cameraX * std::cos(playerDir + M_PI / 2.0f);
                float rayDirY = std::sin(playerDir) + cameraX * std::sin(playerDir + M_PI / 2.0f);
                int mapX = int(playerX), mapY = int(playerY);
                float deltaDistX = std::abs(1.0f / rayDirX), deltaDistY = std::abs(1.0f / rayDirY);
                int stepX, stepY; float sideDistX, sideDistY;
                if (rayDirX < 0) { stepX = -1; sideDistX = (playerX - mapX) * deltaDistX; }
                else { stepX = 1; sideDistX = (mapX + 1.0f - playerX) * deltaDistX; }
                if (rayDirY < 0) { stepY = -1; sideDistY = (playerY - mapY) * deltaDistY; }
                else { stepY = 1; sideDistY = (mapY + 1.0f - playerY) * deltaDistY; }
                int hit = 0, side;
                while (hit == 0) { if (sideDistX < sideDistY) { sideDistX += deltaDistX; mapX += stepX; side = 0; } else { sideDistY += deltaDistY; mapY += stepY; side = 1; } if (worldMap[mapY][mapX] > 0)hit = 1; }
                float perpWallDist = (side == 0) ? (mapX - playerX + (1.0f - stepX) / 2.0f) / rayDirX : (mapY - playerY + (1.0f - stepY) / 2.0f) / rayDirY;
                zBuffer[x] = perpWallDist;
                int wallType = worldMap[mapY][mapX];
                float r = (wallType == 9) ? 1.0f : ((wallType == 1) ? 0.4f : 0.6f); float g = r, b = (wallType == 9) ? 0.0f : r;
                if (side == 1) { r *= 0.7f; g *= 0.7f; b *= 0.7f; }
                float wallX = (side == 0) ? playerY + perpWallDist * rayDirY : playerX + perpWallDist * rayDirX; wallX -= floor(wallX);
                float texX = wallX; if (side == 0 && rayDirX > 0) texX = 1.0f - texX; if (side == 1 && rayDirY < 0) texX = 1.0f - texX;
                float lineHeight = screenHeight / perpWallDist; float drawStart = -lineHeight / 2.0f + screenHeight / 2.0f; float drawEnd = lineHeight / 2.0f + screenHeight / 2.0f;
                float ndcS = 1.0f - 2.0f * drawStart / screenHeight; float ndcE = 1.0f - 2.0f * drawEnd / screenHeight;
                float xL = 2.0f * x / screenWidth - 1.0f; float xR = 2.0f * (x + 1) / screenWidth - 1.0f;
                vertices.insert(vertices.end(), { xL,ndcS,r,g,b,texX,1.0f, xR,ndcS,r,g,b,texX,1.0f, xR,ndcE,r,g,b,texX,0.0f });
                vertices.insert(vertices.end(), { xL,ndcS,r,g,b,texX,1.0f, xR,ndcE,r,g,b,texX,0.0f, xL,ndcE,r,g,b,texX,0.0f });
            }

            glActiveTexture(GL_TEXTURE4);
            for (auto it = hitMarkers.begin(); it != hitMarkers.end();) { it->life -= 1.0f; if (it->life <= 0)it = hitMarkers.erase(it); else ++it; }
            for (const auto& m : hitMarkers) { /* rendering */ }

            // Renderowanie Spritów
            std::vector<Sprite> spritesToRender;
            for (const auto& s : sprites)spritesToRender.push_back(s);
            for (const auto& w : weapons)if (!w.isCollected) { Sprite t; t.x = w.x; t.y = w.y; t.dist = (playerX - w.x) * (playerX - w.x) + (playerY - w.y) * (playerY - w.y); t.isWeapon = true; t.type = w.type; spritesToRender.push_back(t); }
            std::sort(spritesToRender.begin(), spritesToRender.end(), [](const Sprite& a, const Sprite& b) {return a.dist > b.dist; });
            float invDet = 1.0f / (std::cos(playerDir + M_PI / 2) * std::sin(playerDir) - std::cos(playerDir) * std::sin(playerDir + M_PI / 2));
            for (const auto& s : spritesToRender) {
                float spriteX = s.x - playerX; float spriteY = s.y - playerY;
                float transformX = invDet * (std::sin(playerDir) * spriteX - std::cos(playerDir) * spriteY);
                float transformY = invDet * (-std::sin(playerDir + M_PI / 2) * spriteX + std::cos(playerDir + M_PI / 2) * spriteY);
                if (transformY > 0.1f) {
                    int sScreenX = int(screenWidth / 2.0f * (1.0f + transformX / transformY));
                    float scale = 1.0f; if (s.isWeapon && s.type == 0)scale = 0.5f;
                    int sH = abs(int((screenHeight / transformY) * scale)); int sW = sH / 2;
                    int dSX = sScreenX - sW / 2; int dEX = sScreenX + sW / 2;
                    for (int stripe = dSX; stripe < dEX; stripe++) {
                        if (stripe >= 0 && stripe < screenWidth && transformY < zBuffer[stripe]) {
                            float texX = (float)(stripe - dSX) / sW;
                            float ndcS = 1.0f - 2.0f * (screenHeight / 2 - sH / 2) / screenHeight;
                            float ndcE = 1.0f - 2.0f * (screenHeight / 2 + sH / 2) / screenHeight;
                            float xL = 2.0f * stripe / screenWidth - 1.0f; float xR = 2.0f * (stripe + 1) / screenWidth - 1.0f;
                            float b_spr = 1.0f; if (s.isWeapon) { b_spr = (s.type == WEAPON_SHOTGUN) ? 9.0f : 2.0f; }
                            vertices.insert(vertices.end(), { xL,ndcS,1.0f,1.0f,b_spr,texX,1.0f, xR,ndcS,1.0f,1.0f,b_spr,texX,1.0f, xR,ndcE,1.0f,1.0f,b_spr,texX,0.0f });
                            vertices.insert(vertices.end(), { xL,ndcS,1.0f,1.0f,b_spr,texX,1.0f, xR,ndcE,1.0f,1.0f,b_spr,texX,0.0f, xL,ndcE,1.0f,1.0f,b_spr,texX,0.0f });
                        }
                    }
                }
            }

            // HUD
            if (currentWeapon == 1) { glActiveTexture(GL_TEXTURE7); drawQuad2D(vertices, 0.0f, -0.7f, 0.35f, 0.5f, 7.0f); }
            else if (currentWeapon == 2) { glActiveTexture(GL_TEXTURE9); drawQuad2D(vertices, 0.15f, -0.4f, 0.35f, 0.5f, 10.0f); }
            float barColor = 8.0f;
            vertices.insert(vertices.end(), { -1.0f,-0.75f,1.0f,1.0f,barColor,0.0f,0.0f, 1.0f,-0.75f,1.0f,1.0f,barColor,0.0f,0.0f, 1.0f,-1.0f,1.0f,1.0f,barColor,0.0f,0.0f });
            vertices.insert(vertices.end(), { -1.0f,-0.75f,1.0f,1.0f,barColor,0.0f,0.0f, 1.0f,-1.0f,1.0f,1.0f,barColor,0.0f,0.0f, -1.0f,-1.0f,1.0f,1.0f,barColor,0.0f,0.0f });
            glActiveTexture(GL_TEXTURE3); float fSize = 0.05f;
            drawText(vertices, -0.95f, -0.90f, fSize, "HP: " + std::to_string(playerHealth) + "%");

            // --- WYŒWIETLANIE AMUNICJI W HUD ---
            if (currentWeapon == 1) {
                drawText(vertices, -0.25f, -0.90f, fSize, "WEAPON: PISTOL");
                drawText(vertices, 0.7f, -0.90f, fSize, "AMMO: " + std::to_string(ammoPistol));
            }
            else if (currentWeapon == 2) {
                drawText(vertices, -0.25f, -0.90f, fSize, "WEAPON: SHOTGUN");
                drawText(vertices, 0.7f, -0.90f, fSize, "AMMO: " + std::to_string(ammoShotgun));
            }
            else { drawText(vertices, -0.25f, -0.90f, fSize, "WEAPON: KNIFE"); }

        }
        else {
            glUniform1i(useTextureLoc, 0);
            drawGameOverText(vertices);
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