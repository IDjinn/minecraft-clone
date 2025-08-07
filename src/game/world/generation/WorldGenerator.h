//
// Created by Luke on 07/08/2025.
//

#ifndef MINECRAFT_WORLDGENERATOR_H
#define MINECRAFT_WORLDGENERATOR_H
#include <memory>

#include "../World.h"


class WorldGenerator {
    static Chunk build_chunk(uint8_t index);

public:
    static std::unique_ptr<World> generateWorld();
};


#endif //MINECRAFT_WORLDGENERATOR_H
