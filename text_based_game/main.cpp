#include "../game/game_engine/game_engine.hpp"
#include <iostream>

int main() {
    game_engine engine;

    std::cout << "Welcome to The Labyrinth of Echoes!\n";
    std::cout << "A fractured realm where ancient magic and steampunk technology coexist.\n";
    std::cout << "Type 'help' for a list of commands.\n\n";

    engine.initialize();
    engine.run();

    return 0;
}