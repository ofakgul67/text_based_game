#ifndef JSON_LOADER_HPP
#define JSON_LOADER_HPP

#include "../world/world.hpp"
#include "../includes.hpp"
#include <string>
#include "json.hpp"

using json = nlohmann::json;

class json_loader {
public:
    bool load_game_data(const std::string& filename, world& game_world);

private:
    void load_game_config(const json& config, world& game_world);
    void load_world_state(const json& world_state, world& game_world);
    void load_player(const json& player_data, world& game_world);
    void load_npcs(const json& npcs_data, world& game_world);
    void load_items(const json& items_data, world& game_world);
    void load_rooms(const json& rooms_data, world& game_world);

    void setup_npc_behaviors(std::shared_ptr<npc>& npc_ptr, const json& npc_data, world& game_world);
    void setup_dialogue(std::shared_ptr<npc>& npc_ptr, const std::string& state_name, const json& dialogue_data);
};

#endif 
