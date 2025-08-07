//
// Created by Luke on 07/08/2025.
//

#include "Block.h"

#include "../WorldConstants.h"

void Block::setIndex(const uint64_t index) {
    this->i = index;
}

void Block::setBlockType(const uint8_t type) {
    this->blockType = type;
}

uint8_t Block::index() const {
    return this->i;
}

uint8_t Block::block_type() const {
    return blockType;
}

uint8_t Block::x() const {
    return this->i % WORLD_SIZE_X;
}

uint8_t Block::y() const {
    return (this->i / WORLD_SIZE_X) % WORLD_SIZE_Y;
}

uint8_t Block::z() const {
    return this->i / (WORLD_SIZE_X * WORLD_SIZE_Y);
}
