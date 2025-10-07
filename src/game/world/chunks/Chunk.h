//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_CHUNK_H
#define MINECRAFT_CHUNK_H
#include <array>
#include <memory>

#include "ChunkState.h"
#include "../World.h"
#include "../blocks/Block.h"
#include "../WorldConstants.h"

struct World;


struct Chunk {
    const int32_t id;
    std::array<Block, CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z> blocks{};
    ChunkState state = ChunkState::UNKNOWN;
    std::shared_ptr<World> world;

    Chunk(int32_t id, const std::shared_ptr<World> &world_ptr);

    ~Chunk();

    Chunk(const Chunk &) = delete;

    Chunk &operator=(const Chunk &) = delete;

    [[nodiscard]] std::vector<float> generate_mesh(bool ignore_neighbors) const;

    [[nodiscard]] const Block *get_block(int x, int y, int z) const;

    [[nodiscard]] const Block *get_block(uint8_t index) const;

    [[nodiscard]] ChunkState getState() const;

    void set_state(ChunkState newState);

    static constexpr uint32_t block_index(const uint32_t x, const uint32_t y, const uint32_t z) {
        return x + CHUNK_SIZE_X * (y + CHUNK_SIZE_Y * z);
    }
};


#endif //MINECRAFT_CHUNK_H