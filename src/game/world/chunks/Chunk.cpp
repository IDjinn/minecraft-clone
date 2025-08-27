//
// Created by Luke on 07/08/2025.
//

#include "Chunk.h"

#include "../World.h"

Chunk::~Chunk() {
}

std::unique_ptr<std::vector<float> > Chunk::generate_visible_vertices() {
    auto visibleVertices = std::make_unique<std::vector<float> >();
    visibleVertices->reserve(WORLD_RENDER_VERTICES_RESERVE);

    auto world = world_ptr.lock();
    ASSERT_DEBUG(world, "world is invalid");

    const auto [chunk_x, chunk_y, chunk_z] = chunk_id_to_world_coordinates(this->id);
    WHEN_DEBUG(const auto chunk_id = world_coords_to_chunk_id({chunk_x, chunk_y, chunk_z}));
    ASSERT_DEBUG(this->id == chunk_id, "Mismatch chunking id => coordinates");

    for (auto y = 0; y < CHUNK_SIZE_Y; y++) {
        for (auto x = 0; x < CHUNK_SIZE_X; x++) {
            for (auto z = 0; z < CHUNK_SIZE_Z; z++) {
                auto index = Chunk::block_index(x, y, z);
                if (auto &block = this->blocks[index]; block.block_type() == BlockType::AIR) continue;

                for (auto face = 0; face < 6; ++face) {
                    auto nx = x + directions[face][0];
                    auto ny = y + directions[face][1];
                    auto nz = z + directions[face][2];

                    auto neighborIsSolid = false;
                    const auto world_position_current_block = glm::vec3(chunk_x + x, chunk_y + y, chunk_z + z);
                    if (nx >= 0 && nx < CHUNK_SIZE_X &&
                        ny >= 0 && ny < CHUNK_SIZE_Y &&
                        nz >= 0 && nz < CHUNK_SIZE_Z) {
                        auto neighbor_block_index = Chunk::block_index(nx, ny, nz);
                        neighborIsSolid = this->blocks[neighbor_block_index].block_type() != BlockType::AIR;
                    } else {
                        auto neighbor_x = x + directions[face][0];
                        auto neighbor_y = y + directions[face][1];
                        auto neighbor_z = z + directions[face][2];
                        auto neighbor_chunk_id = world_coords_to_chunk_id(
                            {chunk_x + neighbor_x, chunk_y + neighbor_y, chunk_z + neighbor_z}
                        );

                        if (auto it = world->chunks.find(neighbor_chunk_id); it != world->chunks.end()) {
                            auto &neighbor_chunk = it->second;
                            auto neighbor_block_x = (neighbor_x + CHUNK_SIZE_X) % CHUNK_SIZE_X;
                            auto neighbor_block_y = (neighbor_y + CHUNK_SIZE_Y) % CHUNK_SIZE_Y;
                            auto neighbor_block_z = (neighbor_z + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;
                            auto neighbor_index = Chunk::block_index(
                                neighbor_block_x, neighbor_block_y, neighbor_block_z);
                            neighborIsSolid = neighbor_chunk->blocks[neighbor_index].block_type() != BlockType::AIR;
                        }
                    }

                    if (neighborIsSolid) continue;
                    for (int i = 0; i < 30; i += 5) {
                        auto vx = faceVertices[face][i] + world_position_current_block.x;
                        auto vy = faceVertices[face][i + 1] + world_position_current_block.y;
                        auto vz = faceVertices[face][i + 2] + world_position_current_block.z;
                        auto u = faceVertices[face][i + 3];
                        auto v = faceVertices[face][i + 4];
                        visibleVertices->insert(visibleVertices->end(), {vx, vy, vz, u, v});
                    }
                }
            }
        }
    }

    return visibleVertices;
}

const Block *Chunk::getBlock(const int x, const int y, const int z) const {
    return getBlock(block_index(x, y, z));
}

const Block *Chunk::getBlock(const uint8_t index) const {
    return &blocks[index];
}

ChunkState Chunk::getState() const {
    return state;
}

void Chunk::setState(const ChunkState newState) {
    this->state = newState;
}

void Chunk::set_index(uint8_t chunk_index) {
    this->id = chunk_index;
}
