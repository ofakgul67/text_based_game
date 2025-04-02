#include "room.hpp"

room::room(const std::string& room_id) : id(room_id), has_visited(false) {}

void room::set_name(const std::string& room_name) {
    name = room_name;
}

std::string room::get_name() const {
    return name;
}

void room::set_short_description(const std::string& desc) {
    short_description = desc;
}

std::string room::get_short_description() const {
    return short_description;
}

void room::set_long_description(const std::string& desc) {
    long_description = desc;
}

std::string room::get_long_description() const {
    return long_description;
}

void room::set_type(const std::string& room_type) {
    type = room_type;
}

std::string room::get_type() const {
    return type;
}

std::string room::get_id() const {
    return id;
}

void room::add_connection(const std::string& direction, const std::string& room_id, const std::string& required_item) {
    connections[direction] = room_connection(room_id, required_item);
}

void room::unlock_connection(const std::string& direction) {
    auto it = connections.find(direction);
    if (it != connections.end()) {
        it->second.requires_ = ""; 
    }
}

const std::unordered_map<std::string, room_connection>& room::get_connections() const {
    return connections;
}

void room::add_feature(const std::string& feature) {
    features.push_back(feature);
}

const std::vector<std::string>& room::get_features() const {
    return features;
}

void room::add_puzzle(const puzzle& new_puzzle) {
    puzzles.push_back(new_puzzle);
}

const std::vector<puzzle>& room::get_puzzles() const {
    return puzzles;
}

bool room::solve_puzzle(const std::string& puzzle_id) {
    for (auto& puzzle : puzzles) {
        if (puzzle.id == puzzle_id) {
            puzzle.solved = true;
            return true;
        }
    }
    return false;
}

bool room::visited() const {
    return has_visited;
}

void room::set_visited(bool visited) {
    has_visited = visited;
}
