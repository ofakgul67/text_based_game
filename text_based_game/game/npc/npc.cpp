#include "npc.hpp"
#include "../world/world.hpp"
#include <iostream>
#include <random>
#include <chrono>

npc::npc(const std::string& npc_id) : character(npc_id), state("initial") {}

void npc::set_role(const std::string& npc_role) {
    role = npc_role;
}

std::string npc::get_role() const {
    return role;
}

void npc::set_state(const std::string& npc_state) {
    state = npc_state;
}

std::string npc::get_state() const {
    return state;
}

void npc::add_behavior(const std::string& state_name,
    std::function<void(world&, player&)> behavior) {
    behavior_states[state_name] = behavior;
}

void npc::add_dialogue_tree(const std::string& state_name, const std::string& tree_id,
    const dialogue_node& node) {
    dialogue_trees[state_name][tree_id] = node;
}

const dialogue_node* npc::get_dialogue_node(const std::string& state_name,
    const std::string& node_id) const {
    auto state_it = dialogue_trees.find(state_name);
    if (state_it != dialogue_trees.end()) {
        auto node_it = state_it->second.find(node_id);
        if (node_it != state_it->second.end()) {
            return &node_it->second;
        }
    }

    return nullptr;
}

std::string npc::get_greeting() const {
    const auto* node = get_dialogue_node(state, "greeting");
    if (node) {
        return node->npc_text;
    }

    return "Hello.";
}

void npc::update(world& game_world, player& player) {
    auto behavior_it = behavior_states.find(state);
    if (behavior_it != behavior_states.end()) {
        behavior_it->second(game_world, player);
    }
    else {
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed);
        std::uniform_int_distribution<int> distribution(0, 10);

        int action = distribution(generator);

        if (action < 2) { 
            auto room_ptr = game_world.get_room(current_room);
            if (room_ptr) {
                const auto& connections = room_ptr->get_connections();
                if (!connections.empty()) {
                    std::vector<std::string> directions;
                    for (const auto& conn : connections) {
                        if (conn.second.requires_.empty()) { 
                            directions.push_back(conn.first);
                        }
                    }

                    if (!directions.empty()) {
                        std::uniform_int_distribution<int> dir_dist(0, directions.size() - 1);
                        std::string direction = directions[dir_dist(generator)];
                        std::string new_room = connections.at(direction).room_id;

                        if (player.get_current_room() == current_room) {
                            std::cout << name << " leaves to the " << direction << "." << std::endl;
                        }

                        current_room = new_room;

                        if (player.get_current_room() == current_room) {
                            std::cout << name << " enters." << std::endl;
                        }
                    }
                }
            }
        }
    }
}

std::string npc::talk(const std::string& option_index, world& game_world, player& player) {
    std::string node_id = game_world.get_game_flag("has_met_" + id) ? "return_visit" : "first_interaction";
    game_world.set_game_flag("has_met_" + id, true);

    const auto* node = get_dialogue_node(state, node_id);
    if (!node) {
        return "The " + name + " doesn't respond.";
    }

    if (option_index.empty()) {
        std::string result = name + ": \"" + node->npc_text + "\"\n\n";
        result += "What do you say?\n";

        for (size_t i = 0; i < node->options.size(); ++i) {
            result += std::to_string(i + 1) + ": " + node->options[i].text + "\n";
        }

        return result;
    }
    else {
        int choice_idx;
        try {
            choice_idx = std::stoi(option_index) - 1;
        }
        catch (...) {
            return "Invalid choice. Please select a number.";
        }

        if (choice_idx < 0 || choice_idx >= static_cast<int>(node->options.size())) {
            return "Invalid choice. Please select a number between 1 and " +
                std::to_string(node->options.size()) + ".";
        }

        const auto& option = node->options[choice_idx];
        std::string result = name + ": \"" + option.response + "\"";

        if (!option.updates_state.empty()) {
            set_state(option.updates_state);
        }

        if (!option.reveals_item.empty()) {
            auto item_ptr = game_world.get_item(option.reveals_item);
            if (item_ptr) {
                item_ptr->set_location(current_room);
                result += "\n\nThe " + name + " reveals " + item_ptr->get_name() + "!";
            }
        }

        if (!option.adds_journal_entry.empty()) {
            result += "\n\n(New journal entry added: " + option.adds_journal_entry + ")";
        }

        if (!option.leads_to.empty()) {
            const auto* next_node = get_dialogue_node(state, option.leads_to);
            if (next_node) {
                result += "\n\n" + name + ": \"" + next_node->npc_text + "\"\n\n";
                result += "What do you say?\n";

                for (size_t i = 0; i < next_node->options.size(); ++i) {
                    result += std::to_string(i + 1) + ": " + next_node->options[i].text + "\n";
                }
            }
        }

        return result;
    }
}
