#ifndef ITEM_HPP
#define ITEM_HPP

#include "../includes.hpp"
#include <string>
#include <vector> 
#include <unordered_map>

class item {
private:
    std::string id;
    std::string name;
    std::string description;
    std::string type;
    std::string location; 
    std::unordered_map<std::string, std::string> properties;

public:
    item(const std::string& item_id);

    void set_name(const std::string& item_name);
    std::string get_name() const;

    void set_description(const std::string& desc);
    std::string get_description() const;

    void set_type(const std::string& item_type);
    std::string get_type() const;

    std::string get_id() const;

    void set_location(const std::string& loc);
    std::string get_location() const;

    void set_property(const std::string& key, const std::string& value);
    std::string get_property(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& get_properties() const;

    bool use(const std::string& target);
    bool read();
    std::string examine() const;
};

#endif 
