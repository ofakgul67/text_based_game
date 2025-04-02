#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "../character/character.hpp"
#include "../includes.hpp"
#include <string>
#include <vector>

class player : public character {
private:
    std::vector<std::string> abilities;
    int magic_points;

public:
    player();

    void add_ability(const std::string& ability);
    bool has_ability(const std::string& ability) const;
    const std::vector<std::string>& get_abilities() const;

    void set_magic_points(int points);
    int get_magic_points() const;
    bool use_magic_points(int amount);
};

#endif 
