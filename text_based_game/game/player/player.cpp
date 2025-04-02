#include "player.hpp"
#include <algorithm>

player::player() : character("wanderer"), magic_points(50) {
    set_name("The Wanderer");
    set_description("An amnesiac with latent magical abilities");
}

void player::add_ability(const std::string& ability) {
    abilities.push_back(ability);
}

bool player::has_ability(const std::string& ability) const {
    return std::find(abilities.begin(), abilities.end(), ability) != abilities.end();
}

const std::vector<std::string>& player::get_abilities() const {
    return abilities;
}

void player::set_magic_points(int points) {
    magic_points = points;
}

int player::get_magic_points() const {
    return magic_points;
}

bool player::use_magic_points(int amount) {
    if (magic_points >= amount) {
        magic_points -= amount;
        return true;
    }
    return false;
}
