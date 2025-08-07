//
// Created by Luke on 07/08/2025.
//

#include "WorldGenerator.h"

#include "../../../utils/Assert.h"
#include "../blocks/BlockType.h"


std::unique_ptr<World> WorldGenerator::generateWorld() {
    auto world = std::make_unique<World>(OVERWORLD, glm::vec3(0, 0, 0));

    for (int y = 0; y < WORLD_SIZE_Y; ++y) {
        for (int x = 0; x < WORLD_SIZE_X; ++x) {
            for (int z = 0; z < WORLD_SIZE_Z; ++z) {
                auto chunk_index = World::chunk_index(x,y,z);
                auto chunk = world->getChunk(chunk_index);
                chunk.setIndex(chunk_index);


                for (uint8_t i = 0; i < chunk.blocks.size(); ++i) {
                    auto block = chunk.blocks[i];
                    block.setIndex(i);
                    block.setBlockType(DIRT);

                }
            }
        }
    }

    return world;
}
