#pragma once
#include <vector>
#include <string>
#include <GLFW/glfw3.h>

enum GameState {
    MENU,
    PLAYING
};

struct MenuContext {
    int selectedOption = 0;

    bool inSettings = false;
    int settingsOption = 0;

    int resIndex = 0;
    bool isFullscreen = false;
    float volume = 1.0f;
};

void drawText(std::vector<float>& v, float sx, float sy, float s, const std::string& t);
void drawQuad2D(std::vector<float>& v, float x, float y, float w, float h, float cB);

void updateMenu(GLFWwindow* window, GameState& currentState, MenuContext& menuCtx, bool& shouldClose, void(*resetGameFn)(), int& scrW, int& scrH);
void renderMenu(std::vector<float>& vertices, const MenuContext& menuCtx);