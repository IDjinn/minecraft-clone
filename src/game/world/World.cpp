//
// Created by Luke on 07/08/2025.
//

#include "World.h"

#include "../../utils/Assert.h"


Chunk World::getChunk(const int x, const int y, const int z) const {
    return chunks[chunk_index(x, y, z)];
}

Chunk World::getChunk(uint8_t chunk_index) const {
    return chunks[chunk_index];
}
