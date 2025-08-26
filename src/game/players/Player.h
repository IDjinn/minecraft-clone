//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_PLAYER_H
#define MINECRAFT_PLAYER_H
#include <string>
#include <utility>

#include "GLFW/glfw3.h"
#include "glm/vec3.hpp"


#define CAMERA_SPEED 10.0f

struct Player {
    uint32_t id;
    std::string name;
    glm::vec3 position;

    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    bool spacePressed;
    bool draw_line;

    Player(const Player &) = delete;

    Player &operator=(const Player &) = delete;

    Player(const uint32_t id, std::string name, glm::vec3 position) : id(id), name(std::move(name)),
                                                                      position(position) {
    }

    void processInput(GLFWwindow *window, float deltaTime);
};


#endif //MINECRAFT_PLAYER_H
