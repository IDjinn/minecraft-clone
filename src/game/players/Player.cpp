//
// Created by Luke on 07/08/2025.
//

#include "Player.h"

#include "../world/World.h"
#include "glm/ext/quaternion_geometric.hpp"

Player::Player(const uint32_t id, std::string name, glm::vec3 position) : id(id), name(std::move(name)),
                                                                          position(position) {
}

void Player::on_enter_new_chunk(const int32_t chunk_id) {
    PRINT_DEBUG("player was enter in chunk " << chunk_id);
    if (auto world = this->current_world.lock()) {
        PRINT_DEBUG("world " << static_cast<int>(world->id));
        world->check_chunk_lifetimes(position);
    }
}

void Player::processInput(GLFWwindow *window, float deltaTime) {
    const auto current_chunk_id = get_current_chunk_id();
    const auto cameraSpeed = CAMERA_SPEED * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!spacePressed) {
            draw_line = !draw_line;
            spacePressed = true;
        }
    } else {
        spacePressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= cameraSpeed * cameraFront;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        position += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        position -= cameraSpeed * cameraUp;

    const auto new_chunk_id = get_current_chunk_id();
    if (new_chunk_id != current_chunk_id)
        on_enter_new_chunk(new_chunk_id);
}

int32_t Player::get_current_chunk_id() const {
    return World::chunk_id_from_world_coords({
        static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(position.z)
    });
}