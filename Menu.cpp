#include "Menu.h"
#include "Audio.h"
#include <iostream>

static bool keyUpPressed = false;
static bool keyDownPressed = false;
static bool keyEnterPressed = false;

void updateMenu(GLFWwindow* window, GameState& currentState, MenuContext& menuCtx, bool& shouldClose, void(*resetGameFn)()) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (!keyUpPressed) {
            menuCtx.selectedOption--;
            if (menuCtx.selectedOption < 0) menuCtx.selectedOption = 1;

            playMenuBeep();

            keyUpPressed = true;
        }
    }
    else {
        keyUpPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (!keyDownPressed) {
            menuCtx.selectedOption++;
            if (menuCtx.selectedOption > 1) menuCtx.selectedOption = 0;

            playMenuBeep();

            keyDownPressed = true;
        }
    }
    else {
        keyDownPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (!keyEnterPressed) {
            if (menuCtx.selectedOption == 0) {
                playMenuBeep();
                resetGameFn();
                currentState = PLAYING;
            }
            else if (menuCtx.selectedOption == 1) {
                shouldClose = true;
            }
            keyEnterPressed = true;
        }
    }
    else {
        keyEnterPressed = false;
    }
}

void renderMenu(std::vector<float>& vertices, const MenuContext& menuCtx) {
    drawQuad2D(vertices, 0.0f, 0.0f, 1.0f, 1.0f, 60.0f);

    drawQuad2D(vertices, 0.0f, 0.5f, 0.5f, 0.25f, 61.0f);

    float startY = -0.1f;
    float gap = 0.15f;
    float scale = 0.06f;

    if (menuCtx.selectedOption == 0) {
        drawText(vertices, -0.25f, startY, scale + 0.01f, "> NEW GAME <");
    }
    else {
        drawText(vertices, -0.20f, startY, scale, "NEW GAME");
    }

    if (menuCtx.selectedOption == 1) {
        drawText(vertices, -0.25f, startY - gap, scale + 0.01f, "> EXIT GAME <");
    }
    else {
        drawText(vertices, -0.20f, startY - gap, scale, "EXIT GAME");
    }

    drawText(vertices, -0.55f, -0.8f, 0.03f, "ARROWS TO MOVE, ENTER TO SELECT");
}