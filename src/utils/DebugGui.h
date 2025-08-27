//
// Created by Luke on 26/08/2025.
//

#ifndef MINECRAFT_DEBUGGUI_H
#define MINECRAFT_DEBUGGUI_H

#include <memory>

#include "../render/Render.h"
#include "GLFW/glfw3.h"


struct Player;
struct Render;

struct DebugGui {
    static void setup(GLFWwindow *window);

    static void prepare();

    static void render(const Render *render, const std::weak_ptr<World> &world, const std::shared_ptr<Player> &player);

    static void destroy();
};


#endif //MINECRAFT_DEBUGGUI_H
