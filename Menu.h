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
};

void drawText(std::vector<float>& v, float sx, float sy, float s, const std::string& t);
void drawQuad2D(std::vector<float>& v, float x, float y, float w, float h, float cB);

void updateMenu(GLFWwindow* window, GameState& currentState, MenuContext& menuCtx, bool& shouldClose, void(*resetGameFn)());
void renderMenu(std::vector<float>& vertices, const MenuContext& menuCtx);