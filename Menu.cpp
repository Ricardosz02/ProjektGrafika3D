#include <glad/glad.h> 
#include "Menu.h"
#include "Audio.h"
#include <iostream>
#include <string>
#include <iomanip>

static bool keyUpPressed = false;
static bool keyDownPressed = false;
static bool keyLeftPressed = false;
static bool keyRightPressed = false;
static bool keyEnterPressed = false;

void applyResolution(GLFWwindow* window, int index, int& w, int& h, bool fullscreen) {
    if (index == 0) { w = 1920; h = 1080; }
    else if (index == 1) { w = 1280; h = 720; }
    else if (index == 2) { w = 640; h = 480; }

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
                if (menuCtx.settingsOption < 0) menuCtx.settingsOption = 3;
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
                if (menuCtx.settingsOption > 3) menuCtx.settingsOption = 0;
            }
            keyDownPressed = true;
        }
    }
    else keyDownPressed = false;

    if (menuCtx.inSettings) {
        bool changed = false;

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            if (!keyLeftPressed) {
                playMenuBeep();
                if (menuCtx.settingsOption == 0) {
                    menuCtx.resIndex--;
                    if (menuCtx.resIndex < 0) menuCtx.resIndex = 2;
                    applyResolution(window, menuCtx.resIndex, scrW, scrH, menuCtx.isFullscreen);
                }
                else if (menuCtx.settingsOption == 1) {
                    menuCtx.isFullscreen = !menuCtx.isFullscreen;
                    setWindowMode(window, menuCtx.isFullscreen, scrW, scrH);
                }
                else if (menuCtx.settingsOption == 2) {
                    menuCtx.volume -= 0.1f;
                    if (menuCtx.volume < 0.0f) menuCtx.volume = 0.0f;
                    setGlobalVolume(menuCtx.volume);
                }
                keyLeftPressed = true;
            }
        }
        else keyLeftPressed = false;

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            if (!keyRightPressed) {
                playMenuBeep();
                if (menuCtx.settingsOption == 0) {
                    menuCtx.resIndex++;
                    if (menuCtx.resIndex > 2) menuCtx.resIndex = 0;
                    applyResolution(window, menuCtx.resIndex, scrW, scrH, menuCtx.isFullscreen);
                }
                else if (menuCtx.settingsOption == 1) {
                    menuCtx.isFullscreen = !menuCtx.isFullscreen;
                    setWindowMode(window, menuCtx.isFullscreen, scrW, scrH);
                }
                else if (menuCtx.settingsOption == 2) {
                    menuCtx.volume += 0.1f;
                    if (menuCtx.volume > 1.0f) menuCtx.volume = 1.0f;
                    setGlobalVolume(menuCtx.volume);
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
                if (menuCtx.selectedOption == 0) {
                    resetGameFn();
                    currentState = PLAYING;
                }
                else if (menuCtx.selectedOption == 1) {
                    menuCtx.inSettings = true;
                    menuCtx.settingsOption = 0;
                }
                else if (menuCtx.selectedOption == 2) {
                    shouldClose = true;
                }
            }
            else {
                if (menuCtx.settingsOption == 3) {
                    menuCtx.inSettings = false;
                }
                else if (menuCtx.settingsOption == 1) {
                    menuCtx.isFullscreen = !menuCtx.isFullscreen;
                    setWindowMode(window, menuCtx.isFullscreen, scrW, scrH);
                }
            }
            keyEnterPressed = true;
        }
    }
    else keyEnterPressed = false;
}

void renderMenu(std::vector<float>& vertices, const MenuContext& menuCtx) {
    drawQuad2D(vertices, 0.0f, 0.0f, 1.0f, 1.0f, 60.0f);
    drawQuad2D(vertices, 0.0f, 0.6f, 0.5f, 0.25f, 61.0f);

    float startY = -0.1f;
    float gap = 0.13f;
    float scale = 0.05f;

    if (!menuCtx.inSettings) {
        if (menuCtx.selectedOption == 0) drawText(vertices, -0.25f, startY, scale + 0.01f, "> NEW GAME <");
        else drawText(vertices, -0.20f, startY, scale, "NEW GAME");

        if (menuCtx.selectedOption == 1) drawText(vertices, -0.25f, startY - gap, scale + 0.01f, "> SETTINGS <");
        else drawText(vertices, -0.20f, startY - gap, scale, "SETTINGS");

        if (menuCtx.selectedOption == 2) drawText(vertices, -0.25f, startY - gap * 2, scale + 0.01f, "> EXIT GAME <");
        else drawText(vertices, -0.20f, startY - gap * 2, scale, "EXIT GAME");
    }
    else {
        drawText(vertices, -0.25f, startY + 0.1f, scale, "--- OPTIONS ---");

        std::string resText;
        if (menuCtx.resIndex == 0) resText = "1920x1080";
        else if (menuCtx.resIndex == 1) resText = "1280x720";
        else resText = "640x480";

        if (menuCtx.settingsOption == 0) drawText(vertices, -0.45f, startY, scale + 0.01f, "> RES: " + resText + " <");
        else drawText(vertices, -0.40f, startY, scale, "RES: " + resText);

        std::string fsText = menuCtx.isFullscreen ? "ON" : "OFF";

        if (menuCtx.settingsOption == 1) drawText(vertices, -0.45f, startY - gap, scale + 0.01f, "> SCREEN: " + fsText + " <");
        else drawText(vertices, -0.40f, startY - gap, scale, "SCREEN: " + fsText);

        int volPercent = (int)(menuCtx.volume * 100);
        std::string volText = std::to_string(volPercent) + "%";

        if (menuCtx.settingsOption == 2) drawText(vertices, -0.45f, startY - gap * 2, scale + 0.01f, "> VOL: " + volText + " <");
        else drawText(vertices, -0.40f, startY - gap * 2, scale, "VOL: " + volText);

        if (menuCtx.settingsOption == 3) drawText(vertices, -0.25f, startY - gap * 3, scale + 0.01f, "> BACK <");
        else drawText(vertices, -0.20f, startY - gap * 3, scale, "BACK");
    }

    drawText(vertices, -0.55f, -0.90f, 0.03f, "ARROWS TO MOVE, ENTER TO SELECT");
}