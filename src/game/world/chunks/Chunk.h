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

using ChunkId = int32_t;

enum class ChunkState {
    UNKNOWN = 0,

    LOADED = 1,
    INITIALIZED = 2,
};

struct Chunk {
    ChunkId id{};
    std::array<Block, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z> blocks{};
    ChunkState state = ChunkState::UNKNOWN;

    Chunk(ChunkId id) : id(id) {
    }

    Chunk(const Chunk &) = delete;

    Chunk &operator=(const Chunk &) = delete;

    [[nodiscard]] const Block *getBlock(const int x, const int y, const int z) const {
        return getBlock(block_index(x, y, z));
    }

    [[nodiscard]] const Block *getBlock(const uint8_t index) const {
        return &blocks[index];
    }

    [[nodiscard]] ChunkState getState() const {
        return state;
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

        this->setState(ChunkState::INITIALIZED);
    }

    void setState(const ChunkState newState) {
        this->state = newState;
    }

    void setIndex(uint8_t chunk_index);

    static constexpr uint32_t block_index(const uint32_t x, const uint32_t y, const uint32_t z) {
        return x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z);
    }
};


#endif //MINECRAFT_CHUNK_H