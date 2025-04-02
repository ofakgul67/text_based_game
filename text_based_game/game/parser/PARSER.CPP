#include "parser.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

parser::parser() {
    verb_handlers["take"] = [](const std::string& obj, player& player, world& world) {
        if (obj.empty()) {
            std::cout << "Take what?" << std::endl;
            return false;
        }

        auto items = world.get_items_in_room(player.get_current_room());
        for (const auto& item_ptr : items) {
            if (to_lower(item_ptr->get_name()) == to_lower(obj) ||
                to_lower(item_ptr->get_id()) == to_lower(obj) ||
                to_lower(item_ptr->get_id()).find(to_lower(obj)) != std::string::npos ||
                to_lower(item_ptr->get_name()).find(to_lower(obj)) != std::string::npos) {

                if (player.add_to_inventory(item_ptr)) {
                    std::cout << "Taken." << std::endl;
                    return true;
                }
                else {
                    std::cout << "You can't carry any more items." << std::endl;
                    return false;
                }
            }
        }

        std::cout << "You don't see that here." << std::endl;
        return false;
        };

    verb_handlers["drop"] = [](const std::string& obj, player& player, world& world) {
        if (obj.empty()) {
            std::cout << "Drop what?" << std::endl;
            return false;
        }

        auto item_ptr = player.get_item_from_inventory(obj);
        if (!item_ptr) {
            for (const auto& inv_item : player.get_inventory()) {
                if (to_lower(inv_item->get_name()) == to_lower(obj)) {
                    item_ptr = inv_item;
                    break;
                }
            }
        }

        if (item_ptr) {
            item_ptr->set_location(player.get_current_room());
            player.remove_from_inventory(item_ptr->get_id());
            std::cout << "Dropped." << std::endl;
            return true;
        }

        std::cout << "You don't have that." << std::endl;
        return false;
        };

    verb_handlers["examine"] = [](const std::string& obj, player& player, world& world) {
        if (obj.empty()) {
            std::cout << "Examine what?" << std::endl;
            return false;
        }

        auto item_ptr = player.get_item_from_inventory(obj);
        if (!item_ptr) {
            for (const auto& inv_item : player.get_inventory()) {
                if (to_lower(inv_item->get_name()) == to_lower(obj)) {
                    item_ptr = inv_item;
                    break;
                }
            }
        }

        if (item_ptr) {
            std::cout << item_ptr->examine() << std::endl;
            return true;
        }

        auto items = world.get_items_in_room(player.get_current_room());
        for (const auto& room_item : items) {
            if (to_lower(room_item->get_name()) == to_lower(obj) || to_lower(room_item->get_id()) == to_lower(obj)) {
                std::cout << room_item->examine() << std::endl;
                return true;
            }
        }

        auto npcs = world.get_npcs_in_room(player.get_current_room());
        for (const auto& npc_ptr : npcs) {
            if (!npc_ptr) continue;

            std::string npc_id_lower = to_lower(npc_ptr->get_id());
            std::string npc_name_lower = to_lower(npc_ptr->get_name());
            std::string obj_lower = to_lower(obj);

            if (npc_id_lower == obj_lower ||
                npc_name_lower == obj_lower ||
                npc_id_lower.find(obj_lower) != std::string::npos ||
                npc_name_lower.find(obj_lower) != std::string::npos) {

                std::cout << npc_ptr->get_description() << std::endl;
                return true;
            }
        }

        if (world.process_special_command("examine", obj, player)) {
            return true;
        }

        std::cout << "You don't see that here." << std::endl;
        return false;
        };

    verb_handlers["use"] = [](const std::string& obj, player& player, world& world) {
        if (obj.empty()) {
            std::cout << "Use what?" << std::endl;
            return false;
        }

        std::string obj_lower = to_lower(obj);
        if (obj_lower.find("clockwork") != std::string::npos && obj_lower.find("key") != std::string::npos) {
            std::string current_room_id = player.get_current_room();
            auto current_room = world.get_room(current_room_id);

            if (current_room && current_room_id == "sanctum_whispers") {
                auto connections = current_room->get_connections();
                auto it = connections.find("north");

                if (it != connections.end() && it->second.requires_ == "clockwork_key") {
                    std::cout << "You use the Clockwork Key to unlock the northern door." << std::endl;
                    current_room->unlock_connection("north");
                    return true;
                }
            }

            std::cout << "You don't see anything to use the key on here." << std::endl;
            return true;
        }

        size_t on_pos = obj.find(" on ");
        if (on_pos != std::string::npos) {
            std::string item1 = obj.substr(0, on_pos);
            std::string item2 = obj.substr(on_pos + 4);

            if (to_lower(item1).find("clockwork") != std::string::npos &&
                to_lower(item1).find("key") != std::string::npos) {

                std::string current_room_id = player.get_current_room();
                auto current_room = world.get_room(current_room_id);

                if (current_room && current_room_id == "sanctum_whispers") {
                    std::cout << "You use the Clockwork Key to unlock the northern door." << std::endl;
                    current_room->unlock_connection("north");
                    return true;
                }
            }

            auto item1_ptr = player.get_item_from_inventory(item1);
            if (!item1_ptr) {
                for (const auto& inv_item : player.get_inventory()) {
                    if (to_lower(inv_item->get_name()) == to_lower(item1)) {
                        item1_ptr = inv_item;
                        break;
                    }
                }
            }

            if (!item1_ptr) {
                std::cout << "You don't have the " << item1 << "." << std::endl;
                return false;
            }

            return item1_ptr->use(item2);
        }

        if (obj_lower.find("echo") != std::string::npos && obj_lower.find("amulet") != std::string::npos) {
            std::cout << "The amulet glows with an inner light. Ghostly images of the past appear, "
                << "showing how the pathways were originally arranged." << std::endl;
            world.set_game_flag("used_echo_amulet", true);
            return true;
        }

        auto item_ptr = player.get_item_from_inventory(obj);
        if (!item_ptr) {
            for (const auto& inv_item : player.get_inventory()) {
                if (to_lower(inv_item->get_name()) == to_lower(obj)) {
                    item_ptr = inv_item;
                    break;
                }
            }
        }

        if (item_ptr) {
            if (player.get_current_room() == "echo_chamber" &&
                (to_lower(item_ptr->get_id()).find("crystal_fragment") != std::string::npos ||
                    to_lower(item_ptr->get_name()).find("crystal fragment") != std::string::npos)) {

                std::cout << "You place the " << item_ptr->get_name() << " on the altar. ";

                bool all_fragments_used = true;
                for (int i = 1; i <= 5; i++) {
                    std::string fragment_id = "crystal_fragment_" + std::to_string(i);
                    if (player.get_item_from_inventory(fragment_id) && fragment_id != item_ptr->get_id()) {
                        all_fragments_used = false;
                        break;
                    }
                }

                if (all_fragments_used) {
                    std::cout << "All five Crystal Fragments are now on the altar. They begin to glow intensely, "
                        << "rising into the air and drawing together. With a flash of light, they merge into the complete Echo Crystal." << std::endl;

                    world.set_game_flag("crystal_restored", true);

                    auto crystal = std::make_shared<item>("echo_crystal");
                    crystal->set_name("Echo Crystal");
                    crystal->set_description("The restored Echo Crystal, pulsing with otherworldly power.");
                    crystal->set_type("artifact");
                    crystal->set_location("echo_chamber");
                    world.add_item(crystal);

                    for (int i = 1; i <= 5; i++) {
                        player.remove_from_inventory("crystal_fragment_" + std::to_string(i));
                    }
                }
                else {
                    std::cout << "It fits perfectly into one of the indentations, but nothing happens yet. "
                        << "It seems all five fragments are needed." << std::endl;
                }

                return true;
            }

            return item_ptr->use("");
        }

        std::cout << "You don't have that." << std::endl;
        return false;
        };

    verb_handlers["get"] = verb_handlers["take"];
    verb_handlers["look at"] = verb_handlers["examine"];
    verb_handlers["inspect"] = verb_handlers["examine"];
    verb_handlers["pick up"] = verb_handlers["take"];
    verb_handlers["answer"] = [](const std::string& obj, player& player, world& world) {
        return world.process_special_command("answer", obj, player);
        };
}

void parser::parse_command(const std::string& input, std::string& verb, std::string& object) const {
    std::string lower_input = to_lower(input);

    if (lower_input.find("look at ") == 0) {
        verb = "examine";  
        object = lower_input.substr(8);
        return;
    }
    else if (lower_input.find("pick up ") == 0) {
        verb = "take";  
        object = lower_input.substr(8);
        return;
    }

    std::istringstream iss(lower_input);

    iss >> verb;

    std::getline(iss, object);
    if (!object.empty() && object[0] == ' ') {
        object = object.substr(1); 
    }
}

bool parser::execute_command(const std::string& verb, const std::string& object, player& player, world& world) const {
    if (world.process_special_command(verb, object, player)) {
        return true;
    }

    if (verb == "n" || verb == "s" || verb == "e" || verb == "w" ||
        verb == "north" || verb == "south" || verb == "east" || verb == "west" ||
        verb == "up" || verb == "down") {

        std::string direction;
        if (verb == "n" || verb == "north") direction = "north";
        else if (verb == "s" || verb == "south") direction = "south";
        else if (verb == "e" || verb == "east") direction = "east";
        else if (verb == "w" || verb == "west") direction = "west";
        else if (verb == "up") direction = "up";
        else if (verb == "down") direction = "down";

        world.move_player(player, direction);
        return true;
    }

    auto it = verb_handlers.find(verb);
    if (it != verb_handlers.end()) {
        return it->second(object, player, world);
    }

    return false;
}
