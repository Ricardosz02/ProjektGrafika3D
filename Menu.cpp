#include <glad/glad.h> 
#include "Menu.h"
#include "Audio.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <sstream>

const int SETTINGS_COUNT = 5;
const int VISIBLE_ROWS = 4;

static bool keyUpPressed = false;
static bool keyDownPressed = false;
static bool keyLeftPressed = false;
static bool keyRightPressed = false;
static bool keyEnterPressed = false;

void applyResolution(GLFWwindow* window, int index, int& w, int& h, bool fullscreen) {
    if (index == 0) { w = 1920; h = 1080; }
    else if (index == 1) { w = 1280; h = 720; }
    else if (index == 2) { w = 640; h = 480; }
    else if (index == 3) { w = 2560; h = 1440; }

    if (fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, w, h, mode->refreshRate);
    }
    else {
        glfwSetWindowSize(window, w, h);
        glViewport(0, 0, w, h);
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowPos(window, (mode->width - w) / 2, (mode->height - h) / 2);
    }
}

void setWindowMode(GLFWwindow* window, bool fullscreen, int w, int h) {
    if (fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, w, h, mode->refreshRate);
    }
    else {
        glfwSetWindowMonitor(window, nullptr, 100, 100, w, h, 0);
        const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwSetWindowPos(window, (mode->width - w) / 2, (mode->height - h) / 2);
    }
    glViewport(0, 0, w, h);
}

void updateMenu(GLFWwindow* window, GameState& currentState, MenuContext& menuCtx, bool& shouldClose, void(*resetGameFn)(), int& scrW, int& scrH) {

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (!keyUpPressed) {
            playMenuBeep();
            if (!menuCtx.inSettings) {
                menuCtx.selectedOption--;
                if (menuCtx.selectedOption < 0) menuCtx.selectedOption = 2;
            }
            else {
                menuCtx.settingsOption--;
                if (menuCtx.settingsOption < 0) {
                    menuCtx.settingsOption = SETTINGS_COUNT - 1;
                    menuCtx.settingsScrollOffset = SETTINGS_COUNT - VISIBLE_ROWS;
                    if (menuCtx.settingsScrollOffset < 0) menuCtx.settingsScrollOffset = 0;
                }
                else if (menuCtx.settingsOption < menuCtx.settingsScrollOffset) {
                    menuCtx.settingsScrollOffset--;
                }
            }
            keyUpPressed = true;
        }
    }
    else keyUpPressed = false;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (!keyDownPressed) {
            playMenuBeep();
            if (!menuCtx.inSettings) {
                menuCtx.selectedOption++;
                if (menuCtx.selectedOption > 2) menuCtx.selectedOption = 0;
            }
            else {
                menuCtx.settingsOption++;
                if (menuCtx.settingsOption >= SETTINGS_COUNT) {
                    menuCtx.settingsOption = 0;
                    menuCtx.settingsScrollOffset = 0;
                }
                else if (menuCtx.settingsOption >= menuCtx.settingsScrollOffset + VISIBLE_ROWS) {
                    menuCtx.settingsScrollOffset++;
                }
            }
            keyDownPressed = true;
        }
    }
    else keyDownPressed = false;

    if (menuCtx.inSettings) {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (!keyLeftPressed) {
                playMenuBeep();
                if (menuCtx.settingsOption == 0) { // RES
                    menuCtx.resIndex--;
                    if (menuCtx.resIndex < 0) menuCtx.resIndex = 3;
                    applyResolution(window, menuCtx.resIndex, scrW, scrH, menuCtx.isFullscreen);
                }
                else if (menuCtx.settingsOption == 1) { // SCREEN
                    menuCtx.isFullscreen = !menuCtx.isFullscreen;
                    setWindowMode(window, menuCtx.isFullscreen, scrW, scrH);
                }
                else if (menuCtx.settingsOption == 2) { // VOLUME
                    menuCtx.volume -= 0.1f;
                    if (menuCtx.volume < 0.0f) menuCtx.volume = 0.0f;
                    setGlobalVolume(menuCtx.volume);
                }
                else if (menuCtx.settingsOption == 3) { // BRIGHTNESS
                    menuCtx.brightness -= 0.1f;
                    if (menuCtx.brightness < 0.1f) menuCtx.brightness = 0.1f; // Minimum 0.1
                }
                keyLeftPressed = true;
            }
        }
        else keyLeftPressed = false;

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            if (!keyRightPressed) {
                playMenuBeep();
                if (menuCtx.settingsOption == 0) { // RES
                    menuCtx.resIndex++;
                    if (menuCtx.resIndex > 3) menuCtx.resIndex = 0;
                    applyResolution(window, menuCtx.resIndex, scrW, scrH, menuCtx.isFullscreen);
                }
                else if (menuCtx.settingsOption == 1) { // SCREEN
                    menuCtx.isFullscreen = !menuCtx.isFullscreen;
                    setWindowMode(window, menuCtx.isFullscreen, scrW, scrH);
                }
                else if (menuCtx.settingsOption == 2) { // VOLUME
                    menuCtx.volume += 0.1f;
                    if (menuCtx.volume > 1.0f) menuCtx.volume = 1.0f;
                    setGlobalVolume(menuCtx.volume);
                }
                else if (menuCtx.settingsOption == 3) { // BRIGHTNESS
                    menuCtx.brightness += 0.1f;
                    if (menuCtx.brightness > 2.0f) menuCtx.brightness = 2.0f; // Maximum 2.0
                }
                keyRightPressed = true;
            }
        }
        else keyRightPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (!keyEnterPressed) {
            playMenuBeep();
            if (!menuCtx.inSettings) {
                if (menuCtx.selectedOption == 0) { resetGameFn(); currentState = PLAYING; }
                else if (menuCtx.selectedOption == 1) {
                    menuCtx.inSettings = true;
                    menuCtx.settingsOption = 0;
                    menuCtx.settingsScrollOffset = 0;
                }
                else if (menuCtx.selectedOption == 2) { shouldClose = true; }
            }
            else {
                if (menuCtx.settingsOption == 4) { menuCtx.inSettings = false; }
            }
            keyEnterPressed = true;
        }
    }
    else keyEnterPressed = false;
}

void renderMenu(std::vector<float>& vertices, const MenuContext& menuCtx) {

    float bgID = 60.0f;
    if (menuCtx.inSettings) {
        bgID = 63.0f;
    }

    drawQuad2D(vertices, 0.0f, 0.0f, 1.0f, 1.0f, bgID);
    drawQuad2D(vertices, 0.0f, 0.6f, 0.5f, 0.25f, 61.0f);

    float startY = 0.0f;
    float gap = 0.20f;

    float mainScale = 0.08f;
    float exitScale = 0.06f;
    float optionsScale = 0.07f;

    auto drawCenteredOption = [&](int index, int selectedIndex, float y, std::string text, float itemScale) {
        float textWidth = text.length() * itemScale * 0.65f;
        float startX = 0.0f - (textWidth / 2.0f);
        drawText(vertices, startX, y, itemScale, text);

        if (index == selectedIndex) {
            float iconSize = 0.05f;
            float iconY = y + (itemScale * 0.3f);
            float pushRight = 0.01f;

            float leftIconPos = startX - iconSize + pushRight;
            drawQuad2D(vertices, leftIconPos, iconY, iconSize, iconSize, 62.0f);

            float rightIconPos = startX + textWidth + iconSize - pushRight - (itemScale * 0.2f);
            drawQuad2D(vertices, rightIconPos, iconY, iconSize, iconSize, 62.0f);
        }
        };

    if (!menuCtx.inSettings) {
        drawCenteredOption(0, menuCtx.selectedOption, startY, "NEW GAME ", mainScale);
        drawCenteredOption(1, menuCtx.selectedOption, startY - gap, "SETTINGS ", mainScale);
        drawCenteredOption(2, menuCtx.selectedOption, startY - gap * 2, "EXIT GAME ", exitScale);
    }
    else {

        drawCenteredOption(-1, -1, startY + 0.15f, "--- OPTIONS ---", mainScale);

        for (int i = 0; i < SETTINGS_COUNT; i++) {
            if (i >= menuCtx.settingsScrollOffset && i < menuCtx.settingsScrollOffset + VISIBLE_ROWS) {
                int relativeIndex = i - menuCtx.settingsScrollOffset;
                float currentY = startY - (relativeIndex * gap);

                std::string text = "";
                if (i == 0) {
                    std::string resText;
                    if (menuCtx.resIndex == 0) resText = "1920x1080 ";
                    else if (menuCtx.resIndex == 1) resText = "1280x720 ";
                    else if (menuCtx.resIndex == 2) resText = "640x480 ";
                    else if (menuCtx.resIndex == 3) resText = "2560x1440 ";

                    text = "RES: " + resText;
                }
                else if (i == 1) {
                    text = "FULLSCREEN: " + std::string(menuCtx.isFullscreen ? "ON " : "OFF ");
                }
                else if (i == 2) {
                    text = "VOLUME: " + std::to_string((int)(menuCtx.volume * 100.0f + 0.5f)) + "% ";
                }
                else if (i == 3) {
                    int bVal = (int)(menuCtx.brightness * 10.0f + 0.5f);
                    std::string valStr;
                    if (bVal < 10) {
                        valStr = " " + std::to_string(bVal);
                    }
                    else {
                        valStr = std::to_string(bVal);
                    }
                    text = "BRIGHTNESS: " + valStr + " ";
                }
                else if (i == 4) {
                    text = "BACK ";
                }
                drawCenteredOption(i, menuCtx.settingsOption, currentY, text, optionsScale);
            }
        }

        if (menuCtx.settingsScrollOffset > 0)
            drawText(vertices, -0.02f, startY + 0.05f, 0.03f, "^");

        if (menuCtx.settingsScrollOffset + VISIBLE_ROWS < SETTINGS_COUNT)
            drawText(vertices, -0.02f, startY - (VISIBLE_ROWS * gap) + 0.05f, 0.03f, "v");
    }

    std::string footer = "ARROWS TO MOVE, ENTER TO SELECT";
    float footerWidth = footer.length() * 0.03f * 0.65f;
    drawText(vertices, 0.0f - (footerWidth / 2.0f), -0.90f, 0.03f, footer);
}