//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_BLOCK_H
#define MINECRAFT_BLOCK_H
#include <cstdint>

class Block {
    uint8_t i;
    uint8_t blockType;

public:
    Block() = default;

    void setIndex(uint64_t index);

    void setBlockType(uint8_t type);

    [[nodiscard]] uint8_t index() const;

    [[nodiscard]] uint8_t block_type() const;

    [[nodiscard]] uint8_t x() const;

    [[nodiscard]] uint8_t y() const;

    [[nodiscard]] uint8_t z() const;
};

#endif //MINECRAFT_BLOCK_H
