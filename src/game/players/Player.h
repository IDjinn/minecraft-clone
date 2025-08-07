//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_PLAYER_H
#define MINECRAFT_PLAYER_H
#include <string>

#include "glm/vec3.hpp"


class Player {
    uint8_t id;
    std::string name;
    glm::vec3 position;
};


#endif //MINECRAFT_PLAYER_H
