#include "item.hpp"
#include <iostream>

item::item(const std::string& item_id) : id(item_id) {}

void item::set_name(const std::string& item_name) {
    name = item_name;
}

std::string item::get_name() const {
    return name;
}

void item::set_description(const std::string& desc) {
    description = desc;
}

std::string item::get_description() const {
    return description;
}

void item::set_type(const std::string& item_type) {
    type = item_type;
}

std::string item::get_type() const {
    return type;
}

std::string item::get_id() const {
    return id;
}

void item::set_location(const std::string& loc) {
    location = loc;
}

std::string item::get_location() const {
    return location;
}

void item::set_property(const std::string& key, const std::string& value) {
    properties[key] = value;
}

std::string item::get_property(const std::string& key) const {
    auto it = properties.find(key);
    if (it != properties.end()) {
        return it->second;
    }
    return "";
}

const std::unordered_map<std::string, std::string>& item::get_properties() const {
    return properties;
}

bool item::use(const std::string& target) {
    std::cout << "You can't use the " << name << " that way." << std::endl;
    return false;
}

bool item::read() {
    if (get_property("readable") == "true") {
        std::cout << get_property("contents") << std::endl;
        return true;
    }
    else {
        std::cout << "You can't read the " << name << "." << std::endl;
        return false;
    }
}

std::string item::examine() const {
    return description;
}
