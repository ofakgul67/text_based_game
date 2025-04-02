#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "../item/item.hpp"
#include "../includes.hpp"
#include <string>
#include <vector>
#include <memory>

class character {
protected:
    std::string id;
    std::string name;
    std::string description;
    std::string current_room;
    int health;
    std::vector<std::shared_ptr<item>> inventory;
    int inventory_size;

public:
    character(const std::string& char_id);
    virtual ~character() = default;

    void set_name(const std::string& char_name);
    std::string get_name() const;

    void set_description(const std::string& desc);
    std::string get_description() const;

    std::string get_id() const;

    void set_current_room(const std::string& room_id);
    std::string get_current_room() const;

    void set_health(int hp);
    int get_health() const;

    void set_inventory_size(int size);
    int get_inventory_size() const;

    bool add_to_inventory(const std::shared_ptr<item>& item);
    bool remove_from_inventory(const std::string& item_id);
    std::shared_ptr<item> get_item_from_inventory(const std::string& item_id) const;
    const std::vector<std::shared_ptr<item>>& get_inventory() const;
    void clear_inventory();

    void display_inventory() const;
};

#endif 
