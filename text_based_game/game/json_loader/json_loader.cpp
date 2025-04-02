#include "json_loader.hpp"
#include <fstream>
#include <iostream>

std::string get_string(const json& j, const std::string& key, const std::string& default_value = "") {
    if (j.contains(key) && j[key].is_string()) {
        return j[key].get<std::string>();
    }
    return default_value;
}

int get_int(const json& j, const std::string& key, int default_value = 0) {
    if (j.contains(key) && j[key].is_number()) {
        return j[key].get<int>();
    }
    return default_value;
}

bool get_bool(const json& j, const std::string& key, bool default_value = false) {
    if (j.contains(key)) {
        if (j[key].is_boolean()) {
            return j[key].get<bool>();
        }
        else if (j[key].is_number()) {
            return j[key].get<int>() != 0;
        }
    }
    return default_value;
}

bool json_loader::load_game_data(const std::string& filename, world& game_world) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    json j;
    file >> j;

    if (j.contains("game_config") && j["game_config"].is_object()) {
        load_game_config(j["game_config"], game_world);
    }

    if (j.contains("world_state") && j["world_state"].is_object()) {
        load_world_state(j["world_state"], game_world);
    }

    if (j.contains("characters") && j["characters"].is_object() &&
        j["characters"].contains("player") && j["characters"]["player"].is_object()) {
        load_player(j["characters"]["player"], game_world);
    }

    if (j.contains("characters") && j["characters"].is_object() &&
        j["characters"].contains("npcs") && j["characters"]["npcs"].is_object()) {
        load_npcs(j["characters"]["npcs"], game_world);
    }

    if (j.contains("items") && j["items"].is_object()) {
        load_items(j["items"], game_world);
    }

    if (j.contains("locations") && j["locations"].is_object()) {
        load_rooms(j["locations"], game_world);
    }

    return true;
}

void json_loader::load_game_config(const json& config, world& game_world) {
    game_world.set_world_name(config["title"]);

    if (config.contains("initial_state")) {
        auto key = std::make_shared<item>("clockwork_key");
        key->set_name("Clockwork Key");
        key->set_description("A brass key used to operate steampunk machinery.");
        key->set_type("key");
        key->set_property("opens", "forge_door,mechanical_chest,airship_engine");
        key->set_property("breakable", "false");
        game_world.add_item(key);

        auto compass = std::make_shared<item>("runed_compass");
        compass->set_name("Runed Compass");
        compass->set_description("Points toward hidden pathways");
        compass->set_type("tool");
        compass->set_property("reveals_secrets", "true");
        compass->set_property("durability", "infinite");
        compass->set_property("usable_in", "all_locations");
        game_world.add_item(compass);

        for (int i = 1; i <= 5; i++) {
            auto fragment = std::make_shared<item>("crystal_fragment_" + std::to_string(i));
            fragment->set_name("Crystal Fragment " + std::to_string(i));
            fragment->set_description("A glowing fragment of the Echo Crystal.");
            fragment->set_type("quest_item");
            game_world.add_item(fragment);
        }

        game_world.set_starting_room(config["initial_state"]["starting_room"]);

        for (const auto& item : config["initial_state"]["starting_inventory"]) {
            game_world.add_starting_item(item);
        }

        game_world.set_player_health(config["initial_state"]["player_health"]);

        for (const auto& [key, value] : config["initial_state"]["game_flags"].items()) {
            if (value.is_boolean()) {
                game_world.set_game_flag(key, value);
            }
            else if (value.is_number()) {
                game_world.set_game_flag(key, value != 0);
            }
        }
    }
}

void json_loader::load_world_state(const json& world_state, world& game_world) {
    game_world.set_world_description(get_string(world_state, "description",
        "A fractured realm where ancient magic and steampunk technology coexist."));

    if (world_state.contains("environment_states") && world_state["environment_states"].is_object()) {
        const auto& env = world_state["environment_states"];
        if (env.contains("day_cycle") && env["day_cycle"].is_array() && !env["day_cycle"].empty()) {
            const auto& cycle = env["day_cycle"][0];
            if (cycle.is_string()) {
                game_world.set_day_cycle(cycle.get<std::string>());
            }
        }

        if (env.contains("weather_types") && env["weather_types"].is_array() && !env["weather_types"].empty()) {
            const auto& weather = env["weather_types"][0];
            if (weather.is_string()) {
                game_world.set_weather(weather.get<std::string>());
            }
        }
    }
}

void json_loader::load_player(const json& player_data, world& game_world) {
    if (player_data.contains("stats") && player_data["stats"].is_object()) {
        const auto& stats = player_data["stats"];
        game_world.set_player_health(get_int(stats, "health", 100));
        game_world.set_player_inventory_size(get_int(stats, "inventory_size", 10));
    }
}

void json_loader::load_npcs(const json& npcs_data, world& game_world) {
    for (const auto& [npc_id, npc_data] : npcs_data.items()) {
        if (!npc_data.is_object()) continue;

        auto npc_ptr = std::make_shared<npc>(npc_id);
        npc_ptr->set_name(get_string(npc_data, "name", npc_id));
        npc_ptr->set_description(get_string(npc_data, "description", "A mysterious figure."));
        npc_ptr->set_role(get_string(npc_data, "role", "unknown"));
        npc_ptr->set_current_room(get_string(npc_data, "initial_location", "sanctum_whispers"));

        setup_npc_behaviors(npc_ptr, npc_data, game_world);

        game_world.add_npc(npc_ptr);
    }
}

void json_loader::setup_npc_behaviors(std::shared_ptr<npc>& npc_ptr, const json& npc_data, world& game_world) {
    std::string initial_state = "initial";

    if (npc_data.contains("states") && npc_data["states"].is_object()) {
        for (const auto& [state_name, state_data] : npc_data["states"].items()) {
            if (!state_data.is_object()) continue;

            if (initial_state == "initial") {
                initial_state = state_name;
            }

            if (state_data.contains("dialogue") && state_data["dialogue"].is_object()) {
                setup_dialogue(npc_ptr, state_name, state_data["dialogue"]);
            }

            npc_ptr->add_behavior(state_name,
                [state_name, npc_name = npc_ptr->get_name()](world& world, player& player) {
            });
        }
    }

    npc_ptr->set_state(initial_state);
}

void json_loader::setup_dialogue(std::shared_ptr<npc>& npc_ptr, const std::string& state_name, const json& dialogue_data) {
    for (const auto& [dialogue_id, dialogue_node_data] : dialogue_data.items()) {
        if (!dialogue_node_data.is_object()) continue;

        dialogue_node node;
        node.npc_text = "...";

        const char* dialogue_keys[] = {
            "automaton", "greeting", "challenge", "threat", "quest",
            "reward", "respect", "revelation", "trading", "fleeing"
        };

        for (const char* key : dialogue_keys) {
            if (dialogue_node_data.contains(key) && dialogue_node_data[key].is_string()) {
                node.npc_text = dialogue_node_data[key].get<std::string>();
                break;
            }
        }

        if (dialogue_node_data.contains("player_options") && dialogue_node_data["player_options"].is_array()) {
            for (const auto& option_data : dialogue_node_data["player_options"]) {
                if (!option_data.is_object()) continue;

                dialogue_option option;
                option.text = get_string(option_data, "text", "...");
                option.response = get_string(option_data, "response", "...");
                option.leads_to = get_string(option_data, "leads_to");
                option.updates_state = get_string(option_data, "updates_state");
                option.reveals_item = get_string(option_data, "reveals_item");
                option.adds_journal_entry = get_string(option_data, "adds_journal_entry");

                node.options.push_back(option);
            }
        }

        npc_ptr->add_dialogue_tree(state_name, dialogue_id, node);
    }
}

void json_loader::load_items(const json& items_data, world& game_world) {
    if (items_data.contains("passive_items") && items_data["passive_items"].is_object()) {
        for (const auto& [item_id, item_data] : items_data["passive_items"].items()) {
            if (!item_data.is_object()) continue;

            auto item_ptr = std::make_shared<item>(item_id);

            item_ptr->set_name(get_string(item_data, "name", item_id));
            item_ptr->set_description(get_string(item_data, "description", "A mysterious item."));
            item_ptr->set_type(get_string(item_data, "type", "misc"));

            if (item_data.contains("properties") && item_data["properties"].is_object()) {
                for (const auto& [key, value] : item_data["properties"].items()) {
                    if (value.is_null()) {
                        item_ptr->set_property(key, "");
                    }
                    else if (value.is_boolean()) {
                        item_ptr->set_property(key, value.get<bool>() ? "true" : "false");
                    }
                    else if (value.is_number()) {
                        item_ptr->set_property(key, std::to_string(value.get<int>()));
                    }
                    else if (value.is_string()) {
                        item_ptr->set_property(key, value.get<std::string>());
                    }
                    else if (value.is_array()) {
                        std::string combined;
                        for (const auto& elem : value) {
                            if (!combined.empty()) combined += ",";
                            if (elem.is_string()) {
                                combined += elem.get<std::string>();
                            }
                            else if (elem.is_number()) {
                                combined += std::to_string(elem.get<int>());
                            }
                            else if (elem.is_boolean()) {
                                combined += elem.get<bool>() ? "true" : "false";
                            }
                            else {
                                combined += "unknown";
                            }
                        }
                        item_ptr->set_property(key, combined);
                    }
                }
            }

            game_world.add_item(item_ptr);
        }
    }

    if (items_data.contains("quest_items") && items_data["quest_items"].is_object()) {
        const auto& quest_items = items_data["quest_items"];
        if (quest_items.contains("crystal_fragments") && quest_items["crystal_fragments"].is_object()) {
            for (const auto& [fragment_id, fragment_data] : quest_items["crystal_fragments"].items()) {
                if (!fragment_data.is_object()) continue;

                std::string item_id = "crystal_fragment_";
                if (fragment_id.length() > 0) {
                    item_id += fragment_id.back();
                }
                else {
                    continue;
                }

                auto item_ptr = game_world.get_item(item_id);
                if (!item_ptr) {
                    item_ptr = std::make_shared<item>(item_id);
                    item_ptr->set_name("Crystal Fragment");
                    item_ptr->set_description("A glowing fragment of the Echo Crystal.");
                    item_ptr->set_type("quest_item");
                    game_world.add_item(item_ptr);
                }

                if (fragment_data.contains("location") && fragment_data["location"].is_string()) {
                    item_ptr->set_location(fragment_data["location"].get<std::string>());
                }
            }
        }
    }
}

void json_loader::load_rooms(const json& rooms_data, world& game_world) {
    for (const auto& [room_id, room_data] : rooms_data.items()) {
        if (!room_data.is_object()) continue;

        auto room_ptr = std::make_shared<room>(room_id);

        room_ptr->set_name(get_string(room_data, "name", room_id));
        room_ptr->set_type(get_string(room_data, "type", "standard"));

        if (room_data.contains("descriptions") && room_data["descriptions"].is_object()) {
            const auto& descriptions = room_data["descriptions"];
            room_ptr->set_short_description(get_string(descriptions, "short", "A nondescript area."));
            room_ptr->set_long_description(get_string(descriptions, "long",
                "You are in a nondescript area. There doesn't seem to be anything special here."));
        }

        if (room_data.contains("connections") && room_data["connections"].is_object()) {
            for (const auto& [direction, connection] : room_data["connections"].items()) {
                if (connection.is_string()) {
                    room_ptr->add_connection(direction, connection.get<std::string>());
                }
                else if (connection.is_object()) {
                    std::string target = get_string(connection, "leads_to");
                    std::string requires_ = get_string(connection, "requires");

                    if (!target.empty()) {
                        room_ptr->add_connection(direction, target, requires_);
                    }
                }
            }
        }

        if (room_data.contains("features") && room_data["features"].is_array()) {
            for (const auto& feature : room_data["features"]) {
                if (feature.is_string()) {
                    room_ptr->add_feature(feature.get<std::string>());
                }
            }
        }

        if (room_data.contains("puzzles") && room_data["puzzles"].is_object()) {
            for (const auto& [puzzle_id, puzzle_data] : room_data["puzzles"].items()) {
                if (!puzzle_data.is_object()) continue;

                puzzle new_puzzle;
                new_puzzle.id = puzzle_id;
                new_puzzle.type = get_string(puzzle_data, "type", "standard");
                new_puzzle.command = puzzle_id; 
                new_puzzle.object = "";

                if (puzzle_data.contains("requires")) {
                    if (puzzle_data["requires"].is_array()) {
                        for (const auto& req : puzzle_data["requires"]) {
                            if (req.is_string()) {
                                new_puzzle.required_items.push_back(req.get<std::string>());
                            }
                        }
                    }
                    else if (puzzle_data["requires"].is_string()) {
                        new_puzzle.required_items.push_back(puzzle_data["requires"].get<std::string>());
                    }
                }

                new_puzzle.solution = get_string(puzzle_data, "solution");
                new_puzzle.success_message = get_string(puzzle_data, "success_message", "You solved the puzzle!");
                new_puzzle.failure_message = get_string(puzzle_data, "failure_message", "That didn't work.");
                if (puzzle_data.contains("reward") && puzzle_data["reward"].is_string()) {
                    new_puzzle.reward_item = puzzle_data["reward"].get<std::string>();
                }

                room_ptr->add_puzzle(new_puzzle);
            }
        }

        game_world.add_room(room_ptr);
    }
}
