#ifndef ROOM_HPP
#define ROOM_HPP

#include "../includes.hpp"
#include <string>
#include <unordered_map>
#include <vector>

struct room_connection {
    std::string room_id;
    std::string requires_; 

    room_connection() = default;
    room_connection(const std::string& id, const std::string& req = "") :
        room_id(id), requires_(req) {}
};

struct puzzle {
    std::string id;
    std::string type;
    std::string command;
    std::string object;
    std::vector<std::string> required_items;
    std::string solution;
    std::string success_message;
    std::string failure_message;
    std::string reward_item;
    std::string unlocks_path;
    std::string sets_flag;
    bool solved;

    puzzle() : solved(false) {}
};

class room {
private:
    std::string id;
    std::string name;
    std::string short_description;
    std::string long_description;
    std::string type;
    std::unordered_map<std::string, room_connection> connections;
    std::vector<std::string> features;
    std::vector<puzzle> puzzles;
    bool has_visited;

public:
    room(const std::string& id);

    void set_name(const std::string& name);
    std::string get_name() const;

    void set_short_description(const std::string& desc);
    std::string get_short_description() const;

    void set_long_description(const std::string& desc);
    std::string get_long_description() const;

    void set_type(const std::string& type);
    std::string get_type() const;

    std::string get_id() const;

    void add_connection(const std::string& direction, const std::string& room_id, const std::string& required_item = "");
    void unlock_connection(const std::string& direction);
    const std::unordered_map<std::string, room_connection>& get_connections() const;

    void add_feature(const std::string& feature);
    const std::vector<std::string>& get_features() const;

    void add_puzzle(const puzzle& new_puzzle);
    const std::vector<puzzle>& get_puzzles() const;
    bool solve_puzzle(const std::string& puzzle_id);

    bool visited() const;
    void set_visited(bool visited);
};

#endif 
