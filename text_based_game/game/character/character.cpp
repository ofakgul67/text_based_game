#include "character.hpp"
#include <iostream>
#include <algorithm>

character::character(const std::string& char_id) :
    id(char_id), health(100), inventory_size(10) {}

void character::set_name(const std::string& char_name) {
    name = char_name;
}

std::string character::get_name() const {
    return name;
}

void character::set_description(const std::string& desc) {
    description = desc;
}

std::string character::get_description() const {
    return description;
}

std::string character::get_id() const {
    return id;
}

void character::set_current_room(const std::string& room_id) {
    current_room = room_id;
}

std::string character::get_current_room() const {
    return current_room;
}

void character::set_health(int hp) {
    health = hp;
}

int character::get_health() const {
    return health;
}

void character::set_inventory_size(int size) {
    inventory_size = size;
}

int character::get_inventory_size() const {
    return inventory_size;
}

bool character::add_to_inventory(const std::shared_ptr<item>& item) {
    if (inventory.size() >= static_cast<size_t>(inventory_size)) {
        return false;
    }

    inventory.push_back(item);
    item->set_location("inventory");
    return true;
}

bool character::remove_from_inventory(const std::string& item_id) {
    auto it = std::find_if(inventory.begin(), inventory.end(),
        [&item_id](const std::shared_ptr<item>& item) {
            return item->get_id() == item_id;
        });

    if (it != inventory.end()) {
        inventory.erase(it);
        return true;
    }

    return false;
}

std::shared_ptr<item> character::get_item_from_inventory(const std::string& item_id) const {
    for (const auto& item_ptr : inventory) {
        if (item_ptr->get_id() == item_id) {
            return item_ptr;
        }
    }

    return nullptr;
}

const std::vector<std::shared_ptr<item>>& character::get_inventory() const {
    return inventory;
}

void character::clear_inventory() {
    inventory.clear();
}

void character::display_inventory() const {
    if (inventory.empty()) {
        std::cout << "You are not carrying anything." << std::endl;
        return;
    }

    std::cout << "You are carrying:" << std::endl;
    for (const auto& item_ptr : inventory) {
        std::cout << "  " << item_ptr->get_name() << std::endl;
    }
}
