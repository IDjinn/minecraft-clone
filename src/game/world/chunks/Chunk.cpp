//
// Created by Luke on 07/08/2025.
//

#include "Chunk.h"

void Chunk::setIndex(uint8_t chunk_index) {
    this->id = chunk_index;
}

void Chunk::setBlock(const int x, const int y, const int z, Block block) {
    setBlock(block_index(x, y, z), block);
}

void Chunk::setBlock(const int index, const Block block) {
    blocks[index] = block;
}
