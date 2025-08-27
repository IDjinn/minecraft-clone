//
// Created by Luke on 26/08/2025.
//

#ifndef MINECRAFT_GLFW_H
#define MINECRAFT_GLFW_H

#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderManager.h"
#include "TextureManager.h"
#include "../game/GameConstants.h"
#include "../game/players/Player.h"
#include "../game/world/World.h"
#include "../utils/DebugGui.h"

struct Render {
    float yaw;
    float pitch;
    float lastX;
    float lastY;
    bool firstMouse;
    bool mouseEnabled;
    glm::vec<3, float> cameraFront;

    unsigned int visibleVAO, visibleVBO;
    unsigned int shaderProgram;
    unsigned int texture;
    GLFWwindow *window;

    float delta_time = 0.0f;
    float lastFrame = 0.0f;

    ShaderManager shaderManager{};
    TextureManager textureManager{};

    std::weak_ptr<World> current_world;

    explicit Render(const std::weak_ptr<World> &current_world);

    void render();

    ~Render();

    [[nodiscard]] bool is_running() const { return !glfwWindowShouldClose(window); }

    void mouse_callback([[maybe_unused]] GLFWwindow *window, double xpos, double ypos);

    void scroll_callback(
        [[maybe_unused]] GLFWwindow *window,
        [[maybe_unused]] double xoffset,
        [[maybe_unused]] double yoffset
    );

    void key_callback(GLFWwindow *window, int key, [[maybe_unused]] int scancode, int action,
                      [[maybe_unused]] int mods);

    static void framebuffer_size_callback([[maybe_unused]] GLFWwindow *window, const int width, const int height);
};


#endif //MINECRAFT_GLFW_H