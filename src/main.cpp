#define STB_IMAGE_IMPLEMENTATION

#include <vector>

#include "game/Game.h"

std::vector<std::string> debug_output;

int main() {
    Game game;
    game.run();
    return 0;
}