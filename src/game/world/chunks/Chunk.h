//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_CHUNK_H
#define MINECRAFT_CHUNK_H
#include <array>
#include <memory>

#include "../blocks/Block.h"
#include "../WorldConstants.h"
#include "../blocks/BlockType.h"

struct Chunk {
    uint64_t id{};
    std::array<Block, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z> blocks{};

    Chunk(uint64_t id) : id(id) {
    }

    Chunk(const Chunk &) = delete;

    Chunk &operator=(const Chunk &) = delete;

    [[nodiscard]] const Block *getBlock(const int x, const int y, const int z) const {
        return getBlock(block_index(x, y, z));
    }

    [[nodiscard]] const Block *getBlock(const uint8_t index) const {
        return &blocks[index];
    }

    void initializeBlocks() {
        for (int x = 0; x < CHUNK_SIZE_X; x++) {
            for (int y = 0; y < CHUNK_SIZE_Y; y++) {
                for (int z = 0; z < CHUNK_SIZE_Z; z++) {
                    int index = block_index(x, y, z);
                    blocks[index].setIndex(index);
                    blocks[index].setBlockType(BlockType::DIRT);
                }
            }
        }
    }

    void setIndex(uint8_t chunk_index);

    static constexpr int block_index(const int x, const int y, const int z) {
        return x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z);
    }

    void setBlock(int x, int y, int z, Block block);

    void setBlock(int index, Block block);
};


#endif //MINECRAFT_CHUNK_H