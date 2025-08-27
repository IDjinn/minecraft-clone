//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_PLAYER_H
#define MINECRAFT_PLAYER_H
#include <string>
#include <utility>

#include "../world/World.h"
#include "../world/chunks/Chunk.h"
#include "GLFW/glfw3.h"
#include "glm/vec3.hpp"


#define CAMERA_SPEED 10.0f

struct World;

struct Player {
    uint32_t id;
    std::string name;
    glm::vec3 position;
    std::weak_ptr<World> current_world;

    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    bool spacePressed{};
    bool draw_line{};

    Player(const Player &) = delete;

    Player &operator=(const Player &) = delete;

    Player(const uint32_t id, std::string name, glm::vec3 position);

    void on_enter_new_chunk(int32_t chunk_id);

    void processInput(GLFWwindow *window, float deltaTime);

    int32_t get_current_chunk_id() const;
};


#endif //MINECRAFT_PLAYER_H