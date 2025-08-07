//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_CHUNK_H
#define MINECRAFT_CHUNK_H
#include <array>
#include <memory>

#include "../blocks/Block.h"

#define CHUNK_SIZE 64

class Chunk {
public:
    uint64_t id;
    std::array<std::unique_ptr<Block>,CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE> blocks = {};

    static constexpr int index(const int x, const int y, const int z) {
        return x + CHUNK_SIZE * (y + CHUNK_SIZE * z);
    }

    [[nodiscard]] std::shared_ptr<Block> getBlock(const int x, const int y, const int z) const {
        return std::shared_ptr<Block>(blocks[index(x, y, z)].get());
    }
};


#endif //MINECRAFT_CHUNK_H