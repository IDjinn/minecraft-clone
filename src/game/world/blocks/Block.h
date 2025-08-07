//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_BLOCK_H
#define MINECRAFT_BLOCK_H
#include <cstdint>

#include "BlockType.h"

struct Block {
    uint8_t x;
    uint8_t y;
    uint8_t z;

    uint8_t blockType;

    Block() = default;

    Block(const int x, const int y, const int z, const BlockType blockType) {
        this->x = x;
        this->y = y;
        this->z = z;
        this->blockType = blockType;
    }
};

#endif //MINECRAFT_BLOCK_H
