//
// Created by Luke on 26/08/2025.
//

#ifndef MINECRAFT_DEBUGGUI_H
#define MINECRAFT_DEBUGGUI_H

#include "GLFW/glfw3.h"

struct DebugGui {
    static void setup(GLFWwindow *window);

    static void prepare();

    static void render(float delta_time);

    static void destroy();
};


#endif //MINECRAFT_DEBUGGUI_H
