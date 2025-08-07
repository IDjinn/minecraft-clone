//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_CHUNK_H
#define MINECRAFT_CHUNK_H
#include <array>
#include <memory>

#include "../blocks/Block.h"
#include "../WorldConstants.h"

struct Chunk {
    uint64_t id;
    std::array<Block, CHUNK_SIZE * CHUNK_SIZE> blocks{};

    [[nodiscard]] Block getBlock(const int x, const int y, const int z) const {
        return blocks[block_index(x, y, z)];
    }

    void setIndex(uint8_t chunk_index);

    static constexpr int block_index(const int x, const int y, const int z) {
        return x + CHUNK_SIZE * (y + CHUNK_SIZE * z);
    }

    void setBlock(int x, int y, int z, Block block);

    void setBlock(int index, Block block);
};


#endif //MINECRAFT_CHUNK_H
