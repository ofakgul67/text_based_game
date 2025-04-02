#ifndef GAME_ENGINE_HPP
#define GAME_ENGINE_HPP

#include "../world/world.hpp"
#include "../parser/parser.hpp"
#include "../player/player.hpp"
#include "../includes.hpp"
#include <string>
#include <vector>
#include <memory>

class game_engine {
private:
    world game_world;
    parser command_parser;
    player player_character;
    bool game_running;
    std::string config_path;

    void fix_game_paths_and_fragments();
    void hide_fragments_until_puzzles_solved();
    void process_command(const std::string& command);
    void print_help() const;
    void print_introduction() const;
    void update_npcs();

public:
    game_engine();
    void initialize();
    void run();
    void save_game(const std::string& filename) const;
    void load_game(const std::string& filename);
};

#endif 
