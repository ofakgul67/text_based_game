#ifndef WORLD_HPP
#define WORLD_HPP

#include "../room/room.hpp"
#include "../item/item.hpp"
#include "../npc/npc.hpp"
#include "../player/player.hpp"
#include "../includes.hpp"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

class world {
private:
    std::string world_name;
    std::string world_description;
    std::unordered_map<std::string, std::shared_ptr<room>> rooms;
    std::unordered_map<std::string, std::shared_ptr<item>> items;
    std::vector<std::shared_ptr<npc>> npcs;
    std::unordered_map<std::string, bool> game_flags;
    std::string starting_room;
    std::vector<std::string> starting_inventory;
    int player_health;
    int player_inventory_size;
    std::string current_day_cycle;
    std::string current_weather;

public:
    world();

    void set_world_name(const std::string& name);
    std::string get_world_name() const;

    void set_world_description(const std::string& desc);
    std::string get_world_description() const;

    void add_room(const std::shared_ptr<room>& new_room);
    std::shared_ptr<room> get_room(const std::string& room_id) const;

    void add_item(const std::shared_ptr<item>& new_item);
    std::shared_ptr<item> get_item(const std::string& item_id) const;
    std::vector<std::shared_ptr<item>> get_items_in_room(const std::string& room_id) const;

    void add_npc(const std::shared_ptr<npc>& new_npc);
    std::shared_ptr<npc> get_npc(const std::string& npc_id) const;
    std::vector<std::shared_ptr<npc>> get_npcs_in_room(const std::string& room_id) const;
    const std::vector<std::shared_ptr<npc>>& get_npcs() const;

    void set_game_flag(const std::string& flag, bool value);
    bool get_game_flag(const std::string& flag) const;
    const std::unordered_map<std::string, bool>& get_game_flags() const;

    void set_starting_room(const std::string& room_id);
    std::string get_starting_room() const;

    void add_starting_item(const std::string& item_id);
    const std::vector<std::string>& get_starting_inventory() const;

    void set_player_health(int health);
    int get_player_health() const;

    void set_player_inventory_size(int size);
    int get_player_inventory_size() const;

    std::string get_room_description(const std::string& room_id, bool include_contents) const;

    void move_player(player& player, const std::string& direction);
    void update_npcs(player& player);
    void update_npc_state(const std::string& npc_id, const std::string& room_id, const std::string& state);
    void ensure_npcs_in_proper_locations();

    void set_day_cycle(const std::string& cycle);
    std::string get_day_cycle() const;

    void set_weather(const std::string& weather);
    std::string get_weather() const;

    bool process_special_command(const std::string& verb, const std::string& object, player& player);
};

#endif 
