#ifndef NPC_HPP
#define NPC_HPP

#include "../character/character.hpp"
#include "../includes.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

class world;
class player;

struct dialogue_option {
    std::string text;
    std::string response;
    std::string leads_to;
    std::string updates_state;
    std::string reveals_item;
    std::string adds_journal_entry;
};

struct dialogue_node {
    std::string npc_text;
    std::vector<dialogue_option> options;
};

class npc : public character {
private:
    std::string role;
    std::string state;
    std::unordered_map<std::string,
        std::function<void(world&, player&)>> behavior_states;
    std::unordered_map<std::string, std::unordered_map<std::string, dialogue_node>> dialogue_trees;

public:
    npc(const std::string& npc_id);

    void set_role(const std::string& npc_role);
    std::string get_role() const;

    void set_state(const std::string& npc_state);
    std::string get_state() const;

    void add_behavior(const std::string& state_name,
        std::function<void(world&, player&)> behavior);

    void add_dialogue_tree(const std::string& state_name, const std::string& tree_id,
        const dialogue_node& node);

    const dialogue_node* get_dialogue_node(const std::string& state_name,
        const std::string& node_id) const;

    std::string get_greeting() const;

    void update(world& game_world, player& player);

    std::string talk(const std::string& option_index, world& game_world, player& player);
};

#endif 
