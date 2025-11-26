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

const float moveSpeed = 0.05f;
const float rotSpeed = 0.03f;
bool gameOver = false;

const float PISTOL_DAMAGE = 25.0f;

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

uniform sampler2D wallTexture;
uniform sampler2D monsterTexture;
uniform sampler2D pistolTexture;
uniform sampler2D fontTexture;
uniform sampler2D hitTexture;
uniform bool useTexture;                

void main() {
    if (useTexture) {
        vec4 texColor;
        
        if (ourColor.b > 3.9) {
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
        FragColor = texColor * vec4(ourColor.r, ourColor.g, ourColor.b, 1.0);
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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        std::cout << "Zaladowano teksture " << path << " (W:" << width << ", H:" << height << ", K:" << nrChannels << ")" << std::endl;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "!!! BLAD LADOWANIA TEKSTURY: " << path << ". Upewnij sie, ze plik jest w katalogu EXE." << std::endl;
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
    float x1 = x - w;
    float y1 = y - h;
    float x2 = x + w;
    float y2 = y + h;

    float r = 1.0f;
    float g = 1.0f;
    float b = colorB;

    vertices.insert(vertices.end(), { x1, y2, r, g, b, 0.0f, 1.0f });
    vertices.insert(vertices.end(), { x2, y2, r, g, b, 1.0f, 1.0f });
    vertices.insert(vertices.end(), { x2, y1, r, g, b, 1.0f, 0.0f });

    vertices.insert(vertices.end(), { x1, y2, r, g, b, 0.0f, 1.0f });
    vertices.insert(vertices.end(), { x2, y1, r, g, b, 1.0f, 0.0f });
    vertices.insert(vertices.end(), { x1, y1, r, g, b, 0.0f, 0.0f });
}

void drawChar(std::vector<float>& vertices, float x, float y, float size, char c) {
    if (c < 32 || c > 126) return;

    int atlasCols = 16;
    int atlasRows = 6;
    int charIndex = c - 32;

    int row = charIndex / atlasCols;
    int col = charIndex % atlasCols;

    float charWidth = 1.0f / atlasCols;
    float charHeight = 1.0f / atlasRows;

    float u1 = col * charWidth;
    float v1 = 1.0f - (row + 1) * charHeight;
    float u2 = (col + 1) * charWidth;
    float v2 = 1.0f - row * charHeight;

    float r = 1.0f, g = 1.0f, b = 3.0f;

    float x1 = x;
    float y1 = y;
    float x2 = x + size;
    float y2 = y + size;

    vertices.insert(vertices.end(), { x1, y2, r, g, b, u1, v2 });
    vertices.insert(vertices.end(), { x2, y2, r, g, b, u2, v2 });
    vertices.insert(vertices.end(), { x2, y1, r, g, b, u2, v1 });

    vertices.insert(vertices.end(), { x1, y2, r, g, b, u1, v2 });
    vertices.insert(vertices.end(), { x2, y1, r, g, b, u2, v1 });
    vertices.insert(vertices.end(), { x1, y1, r, g, b, u1, v1 });
}

void drawText(std::vector<float>& vertices, float startX, float startY, float charWidthNDC, const std::string& text) {
    float currentX = startX;
    float charHeightNDC = charWidthNDC * screenWidth / screenHeight;

    for (char c : text) {
        drawChar(vertices, currentX, startY, charWidthNDC, c);
        currentX += charWidthNDC;
    }
}


int main() {
    GLFWwindow* window;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(screenWidth, screenHeight, "Mini DOOM z Pistoletem", NULL, NULL);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * screenWidth * 6 * 7 * 2, nullptr, GL_DYNAMIC_DRAW);

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

    GLint useTextureLoc = glGetUniformLocation(shaderProgram, "useTexture");
    GLint hitTextureLoc = glGetUniformLocation(shaderProgram, "hitTexture");

    glUseProgram(shaderProgram);

    glUniform1i(glGetUniformLocation(shaderProgram, "wallTexture"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "monsterTexture"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "pistolTexture"), 2);
    glUniform1i(glGetUniformLocation(shaderProgram, "fontTexture"), 3);
    glUniform1i(hitTextureLoc, 4);

    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, wallTextureID);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, monsterTextureID);
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, pistolTextureID);
    glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glActiveTexture(GL_TEXTURE4); glBindTexture(GL_TEXTURE_2D, hitTextureID);
    glActiveTexture(GL_TEXTURE0);

    glClearColor(0.25f, 0.5f, 0.75f, 1.0f);

    std::vector<float> zBuffer(screenWidth);
    std::vector<float> vertices;

    std::cout << "Mini DOOM z Pistoletem - Start." << std::endl;
    switchMap(activeMapIndex);
    initGoblin(10.5f, 10.5f);
    initWeapons();

    static bool spacePressedLastFrame = false;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (hasPistol && glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spacePressedLastFrame) {
            if (ammoCount > 0) {
                ammoCount--;
                std::cout << "STRZAL! Amunicja: " << ammoCount << std::endl;

                float rayDirX = std::cos(playerDir);
                float rayDirY = std::sin(playerDir);

                BulletFlash flash;
                flash.x = playerX + rayDirX * 0.2f;
                flash.y = playerY + rayDirY * 0.2f;
                flash.dirX = rayDirX;
                flash.dirY = rayDirY;
                flash.life = 2.0f;
                bulletFlashes.push_back(flash);

                float perpWallDist = 1e30f;
                int hitSide = 0;
                int hitMapX = -1;
                int hitMapY = -1;
                float hitTexX = 0.0f;

                int mapX = int(playerX);
                int mapY = int(playerY);
                float sideDistX, sideDistY;
                float deltaDistX = (rayDirX == 0.0f) ? 1e30f : std::abs(1.0f / rayDirX);
                float deltaDistY = (rayDirY == 0.0f) ? 1e30f : std::abs(1.0f / rayDirY);
                int stepX, stepY, side, hit = 0;

                if (rayDirX < 0.0f) { stepX = -1; sideDistX = (playerX - mapX) * deltaDistX; }
                else { stepX = 1; sideDistX = (mapX + 1.0f - playerX) * deltaDistX; }
                if (rayDirY < 0.0f) { stepY = -1; sideDistY = (playerY - mapY) * deltaDistY; }
                else { stepY = 1; sideDistY = (mapY + 1.0f - playerY) * deltaDistY; }

                while (hit == 0) {
                    if (sideDistX < sideDistY) {
                        sideDistX += deltaDistX; mapX += stepX; side = 0;
                    }
                    else {
                        sideDistY += deltaDistY; mapY += stepY; side = 1;
                    }
                    if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT && worldMap[mapY][mapX] > 0) hit = 1;
                }

                if (hit) {
                    perpWallDist = (side == 0) ? (mapX - playerX + (1.0f - stepX) / 2.0f) / rayDirX
                        : (mapY - playerY + (1.0f - stepY) / 2.0f) / rayDirY;

                    float hitX = playerX + perpWallDist * rayDirX;
                    float hitY = playerY + perpWallDist * rayDirY;
                    float wallX = (side == 0) ? hitY - std::floor(hitY) : hitX - std::floor(hitX);

                    hitTexX = wallX;
                    if (side == 0 && rayDirX > 0) hitTexX = 1.0f - hitTexX;
                    if (side == 1 && rayDirY < 0) hitTexX = 1.0f - hitTexX;

                    hitSide = side;
                    hitMapX = mapX;
                    hitMapY = mapY;
                }

                float perpGoblinDist = 1e30f;
                bool goblinHit = false;

                if (!sprites.empty() && sprites[0].isAlive) {
                    Sprite& goblin = sprites[0];

                    float spriteX = goblin.x - playerX;
                    float spriteY = goblin.y - playerY;

                    float planeX = std::cos(playerDir + M_PI / 2.0f);
                    float planeY = std::sin(playerDir + M_PI / 2.0f);
                    float dirX = std::cos(playerDir);
                    float dirY = std::sin(playerDir);
                    float invDet = 1.0f / (planeX * dirY - dirX * planeY);

                    float transformX = invDet * (dirY * spriteX - dirX * spriteY);
                    float transformY = invDet * (-planeY * spriteX + planeX * spriteY);

                    if (transformY > 0.0f && std::abs(transformX / transformY) < 0.1f) {
                        perpGoblinDist = transformY;
                        goblinHit = true;
                    }
                }

                if (goblinHit && perpGoblinDist < perpWallDist) {
                    hitGoblin(PISTOL_DAMAGE);
                }
                else if (hit) {
                    HitMarker marker;
                    marker.x_map = (float)hitMapX + 0.5f;
                    marker.y_map = (float)hitMapY + 0.5f;
                    marker.life = 100.0f;
                    marker.side = hitSide;
                    marker.texX = hitTexX;
                    hitMarkers.push_back(marker);
                }

            }
            else {
                std::cout << "KLIK! Brak amunicji." << std::endl;
            }
            spacePressedLastFrame = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
            spacePressedLastFrame = false;
        }


        if (!gameOver) {

            float moveStep = moveSpeed;

            auto checkMove = [&](float dx, float dy) {
                float nx = playerX + dx;
                float ny = playerY + dy;

                int nextMapX = int(nx);
                int nextMapY = int(ny);

                if (nextMapY >= 0 && nextMapY < MAP_HEIGHT && nextMapX >= 0 && nextMapX < MAP_WIDTH) {

                    int targetBlock = worldMap[nextMapY][nextMapX];

                    if (targetBlock == 0) {
                        playerX = nx;
                        playerY = ny;
                    }
                    else if (targetBlock == 9) {
                        if (activeMapIndex == 1) {
                            switchMap(2); activeMapIndex = 2;
                            playerX = 2.5f; playerY = 2.5f;
                            initGoblin(5.5f, 5.5f);
                            initWeapons();
                        }
                        else if (activeMapIndex == 2) {
                            switchMap(1); activeMapIndex = 1;
                            playerX = 2.5f; playerY = 7.5f;
                            initGoblin(10.5f, 10.5f);
                            initWeapons();
                        }
                    }
                }
                };

            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { checkMove(std::cos(playerDir) * moveStep, std::sin(playerDir) * moveStep); }
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { checkMove(-std::cos(playerDir) * moveStep, -std::sin(playerDir) * moveStep); }
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) { playerDir -= rotSpeed; }
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { playerDir += rotSpeed; }

            checkWeaponCollection(playerX, playerY);
            updateSprites(playerX, playerY);

            if (checkCollision(playerX, playerY)) {
                gameOver = true;
                std::cout << "!!! GAME OVER: Z³apany przez Goblina !!!" << std::endl;
            }
        }

        vertices.clear();
        glUseProgram(shaderProgram);

        if (!gameOver) {

            glUniform1i(useTextureLoc, 1);

            glActiveTexture(GL_TEXTURE0);

            for (int x = 0; x < screenWidth; x++) {

                float cameraX = 2.0f * x / screenWidth - 1.0f;
                float rayDirX = std::cos(playerDir) + cameraX * std::cos(playerDir + M_PI / 2.0f);
                float rayDirY = std::sin(playerDir) + cameraX * std::sin(playerDir + M_PI / 2.0f);

                int mapX = int(playerX);
                int mapY = int(playerY);
                float sideDistX, sideDistY;
                float deltaDistX = (rayDirX == 0.0f) ? 1e30f : std::abs(1.0f / rayDirX);
                float deltaDistY = (rayDirY == 0.0f) ? 1e30f : std::abs(1.0f / rayDirY);
                int stepX, stepY, side, hit = 0;

                if (rayDirX < 0.0f) { stepX = -1; sideDistX = (playerX - mapX) * deltaDistX; }
                else { stepX = 1; sideDistX = (mapX + 1.0f - playerX) * deltaDistX; }
                if (rayDirY < 0.0f) { stepY = -1; sideDistY = (playerY - mapY) * deltaDistY; }
                else { stepY = 1; sideDistY = (mapY + 1.0f - playerY) * deltaDistY; }

                float perpWallDist = 0.0f;
                while (hit == 0) {
                    if (sideDistX < sideDistY) {
                        sideDistX += deltaDistX; mapX += stepX; side = 0;
                    }
                    else {
                        sideDistY += deltaDistY; mapY += stepY; side = 1;
                    }
                    if (mapX >= 0 && mapX < MAP_WIDTH && mapY >= 0 && mapY < MAP_HEIGHT && worldMap[mapY][mapX] > 0) hit = 1;
                }

                perpWallDist = (side == 0) ? (mapX - playerX + (1.0f - stepX) / 2.0f) / rayDirX
                    : (mapY - playerY + (1.0f - stepY) / 2.0f) / rayDirY;

                zBuffer[x] = perpWallDist;

                int wallType = worldMap[mapY][mapX];
                float r, g, b;

                switch (wallType) {
                case 1: r = 0.4f; g = 0.4f; b = 0.4f; break;
                case 9: r = 1.0f; g = 1.0f; b = 0.0f; break;
                default: r = 0.6f; g = 0.6f; b = 0.6f; break;
                }

                if (side == 1) { r *= 0.7f; g *= 0.7f; b *= 0.7f; }

                float wallX = (side == 0) ? playerY + perpWallDist * rayDirY : playerX + perpWallDist * rayDirX;
                wallX -= std::floor(wallX);

                float texX = wallX;
                if (side == 0 && rayDirX > 0) texX = 1.0f - texX;
                if (side == 1 && rayDirY < 0) texX = 1.0f - texX;

                float lineHeight = screenHeight / perpWallDist;
                float drawStart = screenHeight / 2.0f - lineHeight / 2.0f;
                float drawEnd = screenHeight / 2.0f + lineHeight / 2.0f;

                float ndcStart = 1.0f - 2.0f * drawStart / screenHeight;
                float ndcEnd = 1.0f - 2.0f * drawEnd / screenHeight;
                float xposL = 2.0f * x / screenWidth - 1.0f;
                float xposR = 2.0f * (x + 1) / screenWidth - 1.0f;

                vertices.insert(vertices.end(), { xposL, ndcStart, r, g, b, texX, 1.0f, xposR, ndcStart, r, g, b, texX, 1.0f, xposR, ndcEnd, r, g, b, texX, 0.0f });
                vertices.insert(vertices.end(), { xposL, ndcStart, r, g, b, texX, 1.0f, xposR, ndcEnd, r, g, b, texX, 0.0f, xposL, ndcEnd, r, g, b, texX, 0.0f });
            }

            for (auto it = hitMarkers.begin(); it != hitMarkers.end(); ) {
                it->life -= 1.0f;
                if (it->life <= 0.0f) { it = hitMarkers.erase(it); }
                else { ++it; }
            }
            glActiveTexture(GL_TEXTURE4);
            for (const auto& marker : hitMarkers) {
                float spriteX = marker.x_map - playerX;
                float spriteY = marker.y_map - playerY;

                float planeX = std::cos(playerDir + M_PI / 2.0f);
                float planeY = std::sin(playerDir + M_PI / 2.0f);
                float dirX = std::cos(playerDir);
                float dirY = std::sin(playerDir);
                float invDet = 1.0f / (planeX * dirY - dirX * planeY);

                float transformX = invDet * (dirY * spriteX - dirX * spriteY);
                float transformY = invDet * (-planeY * spriteX + planeX * spriteY);

                if (transformY > 0.1f) {
                    int markerScreenX = int(screenWidth / 2.0f * (1.0f + transformX / transformY));
                    float lineHeight = screenHeight / transformY;
                    float center_texel = marker.texX;
                    float marker_size_tex = 0.2f;
                    float start_offset_tex = center_texel - marker_size_tex / 2.0f;
                    float end_offset_tex = center_texel + marker_size_tex / 2.0f;

                    int drawStartX = markerScreenX + (int)((start_offset_tex - 0.5f) * lineHeight);
                    int drawEndX = markerScreenX + (int)((end_offset_tex - 0.5f) * lineHeight);

                    int drawStartY = screenHeight / 2 - (int)(lineHeight / 2.0f);
                    int drawEndY = screenHeight / 2 + (int)(lineHeight / 2.0f);

                    for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
                        if (stripe >= 0 && stripe < screenWidth && transformY < zBuffer[stripe]) {
                            float u_marker = (float)(stripe - drawStartX) / (drawEndX - drawStartX);
                            float ndcStart = 1.0f - 2.0f * drawStartY / screenHeight;
                            float ndcEnd = 1.0f - 2.0f * drawEndY / screenHeight;
                            float xposL = 2.0f * stripe / screenWidth - 1.0f;
                            float xposR = 2.0f * (stripe + 1) / screenWidth - 1.0f;
                            float r = 1.0f, g = 1.0f, b = 4.0f;

                            vertices.insert(vertices.end(), { xposL, ndcStart, r, g, b, u_marker, 1.0f, xposR, ndcStart, r, g, b, u_marker, 1.0f, xposR, ndcEnd, r, g, b, u_marker, 0.0f });
                            vertices.insert(vertices.end(), { xposL, ndcStart, r, g, b, u_marker, 1.0f, xposR, ndcEnd, r, g, b, u_marker, 0.0f, xposL, ndcEnd, r, g, b, u_marker, 0.0f });
                        }
                    }
                }
            }

            for (auto it = bulletFlashes.begin(); it != bulletFlashes.end(); ) {
                it->life -= 1.0f;
                if (it->life <= 0.0f) { it = bulletFlashes.erase(it); }
                else { ++it; }
            }
            glActiveTexture(GL_TEXTURE4);
            float r_flash = 1.0f, g_flash = 1.0f, b_flash = 4.0f;
            for (const auto& flash : bulletFlashes) {
                float travel_dist = (2.0f - flash.life) * 0.5f;
                float currentX = flash.x + flash.dirX * travel_dist;
                float currentY = flash.y + flash.dirY * travel_dist;

                float spriteX = currentX - playerX;
                float spriteY = currentY - playerY;

                float planeX = std::cos(playerDir + M_PI / 2.0f);
                float planeY = std::sin(playerDir + M_PI / 2.0f);
                float dirX = std::cos(playerDir);
                float dirY = std::sin(playerDir);
                float invDet = 1.0f / (planeX * dirY - dirX * planeY);

                float transformX = invDet * (dirY * spriteX - dirX * spriteY);
                float transformY = invDet * (-planeY * spriteX + planeX * spriteY);

                if (transformY > 0.1f) {
                    int spriteScreenX = int(screenWidth / 2.0f * (1.0f + transformX / transformY));

                    int spriteHeight = std::abs(int(screenHeight / transformY));
                    int spriteWidth = spriteHeight / 4;

                    int drawStartY = screenHeight / 2 - spriteHeight / 2;
                    int drawEndY = screenHeight / 2 + spriteHeight / 2;
                    int drawStartX = spriteScreenX - spriteWidth / 2;
                    int drawEndX = spriteScreenX + spriteWidth / 2;

                    for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
                        if (stripe >= 0 && stripe < screenWidth && transformY < zBuffer[stripe]) {

                            float texX = (float)(stripe - drawStartX) / spriteWidth;

                            float ndcStart = 1.0f - 2.0f * drawStartY / screenHeight;
                            float ndcEnd = 1.0f - 2.0f * drawEndY / screenHeight;
                            float xposL = 2.0f * stripe / screenWidth - 1.0f;
                            float xposR = 2.0f * (stripe + 1) / screenWidth - 1.0f;

                            vertices.insert(vertices.end(), {
                                xposL, ndcStart, r_flash, g_flash, b_flash, texX, 1.0f,
                                xposR, ndcStart, r_flash, g_flash, b_flash, texX, 1.0f,
                                xposR, ndcEnd,   r_flash, g_flash, b_flash, texX, 0.0f
                                });

                            vertices.insert(vertices.end(), {
                                xposL, ndcStart, r_flash, g_flash, b_flash, texX, 1.0f,
                                xposR, ndcEnd,   r_flash, g_flash, b_flash, texX, 0.0f,
                                xposL, ndcEnd,   r_flash, g_flash, b_flash, texX, 0.0f
                                });
                        }
                    }
                }
            }

            glActiveTexture(GL_TEXTURE0);

            std::vector<Sprite> spritesToRender;

            for (const auto& s : sprites) {
                spritesToRender.push_back(s);
            }

            for (const auto& w : weapons) {
                if (!w.isCollected) {
                    float distSq = (playerX - w.x) * (playerX - w.x) + (playerY - w.y) * (playerY - w.y);

                    Sprite tempSprite;
                    tempSprite.x = w.x;
                    tempSprite.y = w.y;
                    tempSprite.dist = distSq;
                    tempSprite.isWeapon = true;

                    spritesToRender.push_back(tempSprite);
                }
            }

            std::sort(spritesToRender.begin(), spritesToRender.end(), [](const Sprite& a, const Sprite& b) {
                return a.dist > b.dist;
                });

            float planeX = std::cos(playerDir + M_PI / 2.0f);
            float planeY = std::sin(playerDir + M_PI / 2.0f);
            float dirX = std::cos(playerDir);
            float dirY = std::sin(playerDir);
            float invDet = 1.0f / (planeX * dirY - dirX * planeY);

            float r_sprite = 1.0f, g_sprite = 1.0f;

            for (const auto& s : spritesToRender) {
                float spriteX = s.x - playerX;
                float spriteY = s.y - playerY;

                float b_sprite_final = s.isWeapon ? 2.0f : 1.0f;

                float transformX = invDet * (dirY * spriteX - dirX * spriteY);
                float transformY = invDet * (-planeY * spriteX + planeX * spriteY);

                if (transformY > 0.1f) {
                    int spriteScreenX = int(screenWidth / 2.0f * (1.0f + transformX / transformY));

                    int spriteHeight = std::abs(int(screenHeight / transformY));
                    int spriteWidth = std::abs(int(screenHeight / transformY)) / 2;

                    int drawStartY = screenHeight / 2 - spriteHeight / 2;
                    int drawEndY = screenHeight / 2 + spriteHeight / 2;
                    int drawStartX = spriteScreenX - spriteWidth / 2;
                    int drawEndX = spriteScreenX + spriteWidth / 2;

                    for (int stripe = drawStartX; stripe < drawEndX; stripe++) {
                        if (stripe >= 0 && stripe < screenWidth && transformY < zBuffer[stripe]) {

                            float texX = (float)(stripe - drawStartX) / spriteWidth;

                            float ndcStart = 1.0f - 2.0f * drawStartY / screenHeight;
                            float ndcEnd = 1.0f - 2.0f * drawEndY / screenHeight;
                            float xposL = 2.0f * stripe / screenWidth - 1.0f;
                            float xposR = 2.0f * (stripe + 1) / screenWidth - 1.0f;

                            vertices.insert(vertices.end(), {
                                xposL, ndcStart, r_sprite, g_sprite, b_sprite_final, texX, 1.0f,
                                xposR, ndcStart, r_sprite, g_sprite, b_sprite_final, texX, 1.0f,
                                xposR, ndcEnd,   r_sprite, g_sprite, b_sprite_final, texX, 0.0f
                                });

                            vertices.insert(vertices.end(), {
                                xposL, ndcStart, r_sprite, g_sprite, b_sprite_final, texX, 1.0f,
                                xposR, ndcEnd,   r_sprite, g_sprite, b_sprite_final, texX, 0.0f,
                                xposL, ndcEnd,   r_sprite, g_sprite, b_sprite_final, texX, 0.0f
                                });
                        }
                    }
                }
            }

            glActiveTexture(GL_TEXTURE2);
            if (hasPistol) {
                drawQuad2D(vertices, 0.45f, -0.5f, 0.5f, 0.5f, 2.0f);
            }

            glActiveTexture(GL_TEXTURE3);
            std::string ammoText = "AMMO: " + std::to_string(ammoCount);
            drawText(vertices, -0.98f, -0.9f, 0.1f, ammoText);


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

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}