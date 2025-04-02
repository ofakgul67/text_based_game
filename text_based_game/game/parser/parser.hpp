#ifndef PARSER_HPP
#define PARSER_HPP

#include "../player/player.hpp"
#include "../world/world.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include "../includes.hpp"

class parser {
private:
    std::unordered_map<std::string, std::function<bool(const std::string&, player&, world&)>> verb_handlers;

public:
    parser();

    void parse_command(const std::string& input, std::string& verb, std::string& object) const;
    bool execute_command(const std::string& verb, const std::string& object, player& player, world& world) const;
};

#endif 
