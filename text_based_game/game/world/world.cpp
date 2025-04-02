#include "world.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <string>

world::world() :
    player_health(100),
    player_inventory_size(10),
    current_day_cycle("day"),
    current_weather("clear") {}

void world::set_world_name(const std::string& name) {
    world_name = name;
}

std::string world::get_world_name() const {
    return world_name;
}

void world::set_world_description(const std::string& desc) {
    world_description = desc;
}

std::string world::get_world_description() const {
    return world_description;
}

void world::add_room(const std::shared_ptr<room>& new_room) {
    rooms[new_room->get_id()] = new_room;
}

std::shared_ptr<room> world::get_room(const std::string& room_id) const {
    auto it = rooms.find(room_id);
    if (it != rooms.end()) {
        return it->second;
    }
    return nullptr;
}

void world::add_item(const std::shared_ptr<item>& new_item) {
    items[new_item->get_id()] = new_item;
}

std::shared_ptr<item> world::get_item(const std::string& item_id) const {
    auto it = items.find(item_id);
    if (it != items.end()) {
        return it->second;
    }

    std::string normalized_id = item_id;
    std::transform(normalized_id.begin(), normalized_id.end(), normalized_id.begin(),
        [](unsigned char c) { return std::tolower(c); });
    std::replace(normalized_id.begin(), normalized_id.end(), ' ', '_');

    it = items.find(normalized_id);
    if (it != items.end()) {
        return it->second;
    }

    std::string lower_item_id = item_id;
    std::transform(lower_item_id.begin(), lower_item_id.end(), lower_item_id.begin(),
        [](unsigned char c) { return std::tolower(c); });

    for (const auto& pair : items) {
        std::string item_name_lower = pair.second->get_name();
        std::transform(item_name_lower.begin(), item_name_lower.end(), item_name_lower.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (item_name_lower == lower_item_id || item_name_lower.find(lower_item_id) != std::string::npos) {
            return pair.second;
        }
    }

    return nullptr;
}

std::vector<std::shared_ptr<item>> world::get_items_in_room(const std::string& room_id) const {
    std::vector<std::shared_ptr<item>> result;
    for (const auto& pair : items) {
        if (pair.second->get_location() == room_id) {
            result.push_back(pair.second);
        }
    }
    return result;
}

void world::add_npc(const std::shared_ptr<npc>& new_npc) {
    npcs.push_back(new_npc);
}

std::shared_ptr<npc> world::get_npc(const std::string& npc_id) const {
    for (const auto& npc_ptr : npcs) {
        if (npc_ptr->get_id() == npc_id) {
            return npc_ptr;
        }
    }

    std::string lower_npc_id = npc_id;
    std::transform(lower_npc_id.begin(), lower_npc_id.end(), lower_npc_id.begin(),
        [](unsigned char c) { return std::tolower(c); });

    std::replace(lower_npc_id.begin(), lower_npc_id.end(), ' ', '_');
    for (const auto& npc_ptr : npcs) {
        std::string npc_id_lower = npc_ptr->get_id();
        std::transform(npc_id_lower.begin(), npc_id_lower.end(), npc_id_lower.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (npc_id_lower == lower_npc_id) {
            return npc_ptr;
        }
    }

    for (const auto& npc_ptr : npcs) {
        std::string npc_name_lower = npc_ptr->get_name();
        std::transform(npc_name_lower.begin(), npc_name_lower.end(), npc_name_lower.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (npc_name_lower == lower_npc_id || npc_name_lower.find(lower_npc_id) != std::string::npos) {
            return npc_ptr;
        }
    }

    return nullptr;
}

std::vector<std::shared_ptr<npc>> world::get_npcs_in_room(const std::string& room_id) const {
    std::vector<std::shared_ptr<npc>> result;
    for (const auto& npc_ptr : npcs) {
        if (npc_ptr->get_current_room() == room_id) {
            result.push_back(npc_ptr);
        }
    }
    return result;
}

const std::vector<std::shared_ptr<npc>>& world::get_npcs() const {
    return npcs;
}

void world::set_game_flag(const std::string& flag, bool value) {
    game_flags[flag] = value;
}

bool world::get_game_flag(const std::string& flag) const {
    auto it = game_flags.find(flag);
    if (it != game_flags.end()) {
        return it->second;
    }
    return false;
}

const std::unordered_map<std::string, bool>& world::get_game_flags() const {
    return game_flags;
}

void world::set_starting_room(const std::string& room_id) {
    starting_room = room_id;
}

std::string world::get_starting_room() const {
    return starting_room;
}

void world::add_starting_item(const std::string& item_id) {
    starting_inventory.push_back(item_id);
}

const std::vector<std::string>& world::get_starting_inventory() const {
    return starting_inventory;
}

void world::set_player_health(int health) {
    player_health = health;
}

int world::get_player_health() const {
    return player_health;
}

void world::set_player_inventory_size(int size) {
    player_inventory_size = size;
}

int world::get_player_inventory_size() const {
    return player_inventory_size;
}

std::string world::get_room_description(const std::string& room_id, bool include_contents) const {
    auto room_ptr = get_room(room_id);
    if (!room_ptr) {
        return "Error: Room not found.";
    }

    std::stringstream result;
    result << "[" << room_ptr->get_name() << "]\n";

    if (room_ptr->visited() && !include_contents) {
        result << room_ptr->get_short_description();
    }
    else {
        result << room_ptr->get_long_description();
        room_ptr->set_visited(true);
    }

    if (include_contents) {
        auto items_in_room = get_items_in_room(room_id);
        if (!items_in_room.empty()) {
            result << "\n";
            for (const auto& item_ptr : items_in_room) {
                result << "There is " << item_ptr->get_name() << " here.\n";
            }
        }

        auto npcs_in_room = get_npcs_in_room(room_id);
        if (!npcs_in_room.empty()) {
            result << "\n";
            for (const auto& npc_ptr : npcs_in_room) {
                result << "You see " << npc_ptr->get_name() << " here.\n";
            }
        }

        result << "\nExits: ";
        const auto& connections = room_ptr->get_connections();
        if (connections.empty()) {
            result << "none";
        }
        else {
            bool first = true;
            for (const auto& conn : connections) {
                if (!first) {
                    result << ", ";
                }
                result << conn.first;
                first = false;
            }
        }
    }

    return result.str();
}

void world::move_player(player& player, const std::string& direction) {
    auto current_room = get_room(player.get_current_room());
    if (!current_room) {
        std::cout << "Error: Current room not found." << std::endl;
        return;
    }

    auto connections = current_room->get_connections();
    auto it = connections.find(direction);
    if (it == connections.end()) {
        std::cout << "You can't go that way." << std::endl;
        return;
    }

    std::string next_room_id = it->second.room_id;
    std::string required_item = it->second.requires_;

    if (!required_item.empty()) {
        bool has_item = false;
        const auto& inventory = player.get_inventory();
        for (const auto& item_ptr : inventory) {
            if (item_ptr->get_id() == required_item) {
                has_item = true;
                break;
            }
        }

        if (!has_item) {
            std::cout << "You need " << get_item(required_item)->get_name() << " to go that way." << std::endl;
            return;
        }
    }

    player.set_current_room(next_room_id);
    std::cout << get_room_description(next_room_id, true) << std::endl;
}

void world::update_npcs(player& player) {
    for (auto& npc_ptr : npcs) {
        if (!npc_ptr) {
            continue;
        }

        std::string npc_id = npc_ptr->get_id();
        std::string current_location = npc_ptr->get_current_room();
        std::string proper_location = current_location; 

        if (npc_id == "guardian_automaton") {
            proper_location = "sanctum_whispers";
        }
        else if (npc_id == "librarian") {
            proper_location = "archive_shadows";
        }
        else if (npc_id == "gorath") {
            proper_location = "ember_peaks";
        }
        else if (npc_id == "veyra") {
            proper_location = "veyras_airship";
        }
        else if (npc_id == "architect") {
            proper_location = "echo_chamber";
        }

        if (current_location != proper_location) {
            if (player.get_current_room() == current_location) {
                std::cout << npc_ptr->get_name() << " leaves." << std::endl;
            }

            npc_ptr->set_current_room(proper_location);
            if (player.get_current_room() == proper_location) {
                std::cout << npc_ptr->get_name() << " enters." << std::endl;
            }
        }
    }
}

void world::update_npc_state(const std::string& npc_id, const std::string& room_id, const std::string& state) {
    auto npc_ptr = get_npc(npc_id);
    if (npc_ptr) {
        npc_ptr->set_current_room(room_id);
        npc_ptr->set_state(state);
    }
}

void world::set_day_cycle(const std::string& cycle) {
    current_day_cycle = cycle;
}

std::string world::get_day_cycle() const {
    return current_day_cycle;
}

void world::set_weather(const std::string& weather) {
    current_weather = weather;
}

std::string world::get_weather() const {
    return current_weather;
}

bool world::process_special_command(const std::string& verb, const std::string& object, player& player) {
    auto current_room = get_room(player.get_current_room());
    if (!current_room) {
        return false;
    }

    std::string current_room_id = current_room->get_id();

    ensure_npcs_in_proper_locations();

    if (current_room_id == "sanctum_whispers") {
        if (verb == "activate") {
            static std::vector<std::string> rune_sequence;
            static bool puzzle_solved = false;

            if (puzzle_solved) {
                std::cout << "The runes have already been activated." << std::endl;
                return true;
            }

            if (object == "blue" || object == "red" || object == "green") {
                rune_sequence.push_back(object);
                std::cout << "The " << object << " rune glows brightly as you activate it." << std::endl;

                if (rune_sequence.size() == 3) {
                    if (rune_sequence[0] == "blue" && rune_sequence[1] == "red" && rune_sequence[2] == "green") {
                        std::cout << "The combination of runes triggers a mechanism in the wall. "
                            << "A hidden compartment opens, revealing a Clockwork Key!" << std::endl;

                        auto key = get_item("clockwork_key");
                        if (key) {
                            key->set_location(player.get_current_room());
                        }
                        else {
                            auto new_key = std::make_shared<item>("clockwork_key");
                            new_key->set_name("Clockwork Key");
                            new_key->set_description("A brass key used to operate steampunk machinery.");
                            new_key->set_type("key");
                            new_key->set_location(player.get_current_room());
                            add_item(new_key);
                        }

                        puzzle_solved = true;
                        set_game_flag("sanctum_puzzle_solved", true);
                    }
                    else {
                        std::cout << "The runes flash briefly, then fade. That combination didn't work." << std::endl;
                        rune_sequence.clear();
                    }
                }
                return true;
            }
        }

        if ((verb == "examine" || verb == "look") &&
            (object == "western wall" || object == "western" || object == "wall" ||
                object == "walls" || object.find("wall") != std::string::npos)) {
            std::cout << "The western wall is covered in ornate runes. You notice that some of them - "
                << "colored blue, red, and green - seem to react to your presence." << std::endl;
            return true;
        }

        if ((verb == "examine" || verb == "look") &&
            (object == "runes" || object == "glowing runes" || object.find("rune") != std::string::npos)) {
            std::cout << "The glowing runes pulse with an otherworldly light. "
                << "Three runes stand out: one blue, one red, and one green." << std::endl;
            return true;
        }
    }

    if (current_room_id == "clockwork_forge") {
        static bool bridge_puzzle_solved = false;
        static bool large_gear_placed = false;
        static bool medium_gear_placed = false;
        static bool small_gear_placed = false;

        if (get_game_flag("bridge_puzzle_solved")) {
            bridge_puzzle_solved = true;
        }

        auto large_gear = get_item("large_gear");
        if (!large_gear) {
            auto new_gear = std::make_shared<item>("large_gear");
            new_gear->set_name("Large Gear");
            new_gear->set_description("A hefty metal gear that appears to be part of a mechanism.");
            new_gear->set_type("part");
            new_gear->set_location(player.get_current_room());
            add_item(new_gear);
        }

        auto medium_gear = get_item("medium_gear");
        if (!medium_gear) {
            auto new_gear = std::make_shared<item>("medium_gear");
            new_gear->set_name("Medium Gear");
            new_gear->set_description("A medium-sized gear with intricate teeth.");
            new_gear->set_type("part");
            new_gear->set_location(player.get_current_room());
            add_item(new_gear);
        }

        auto small_gear = get_item("small_gear");
        if (!small_gear) {
            auto new_gear = std::make_shared<item>("small_gear");
            new_gear->set_name("Small Gear");
            new_gear->set_description("A small but precisely crafted gear.");
            new_gear->set_type("part");
            new_gear->set_location(player.get_current_room());
            add_item(new_gear);
        }

        auto fragment1 = get_item("crystal_fragment_1");
        if (!fragment1) {
            auto new_fragment = std::make_shared<item>("crystal_fragment_1");
            new_fragment->set_name("Crystal Fragment 1");
            new_fragment->set_description("A glowing fragment of the Echo Crystal.");
            new_fragment->set_type("quest_item");
            if (bridge_puzzle_solved) {
                new_fragment->set_location(player.get_current_room());
            }
            else {
                new_fragment->set_location("hidden");
            }
            add_item(new_fragment);
        }
        else if (bridge_puzzle_solved &&
            fragment1->get_location() != "clockwork_forge" &&
            fragment1->get_location() != "inventory" &&
            fragment1->get_location() != "placed" &&
            fragment1->get_location() != "echo_chamber") {
            fragment1->set_location("clockwork_forge");
        }
        else if (!bridge_puzzle_solved &&
            fragment1->get_location() != "inventory" &&
            fragment1->get_location() != "placed" &&
            fragment1->get_location() != "echo_chamber") {
            fragment1->set_location("hidden");
        }

        if ((verb == "examine" || verb == "look") &&
            (object == "gear bridge" || object == "bridge" || object == "gear_bridge")) {
            if (bridge_puzzle_solved) {
                std::cout << "The gear bridge is now fully operational, providing a sturdy path across the chasm. "
                    << "On the far side, you can see a Crystal Fragment glinting in the light." << std::endl;
            }
            else {
                std::cout << "A massive mechanism spans a chasm in the center of the forge. "
                    << "It appears to be a bridge, but several key gears are missing from its workings. "
                    << "Through the gap, you can see something glittering on the other side." << std::endl;
            }
            return true;
        }

        if ((verb == "examine" || verb == "look") &&
            (object == "mechanical workbench" || object == "workbench" || object == "mechanical_workbench")) {
            std::cout << "A sturdy workbench covered with tools and mechanical parts. "
                << "Various gears of different sizes are scattered across its surface." << std::endl;
            return true;
        }

        if (verb == "use" &&
            (object == "large gear" || object == "large_gear")) {
            std::cout << "You place the large gear into the main mechanism of the bridge. "
                << "It fits perfectly into the central housing." << std::endl;
            large_gear_placed = true;

            auto gear = get_item("large_gear");
            if (gear && player.remove_from_inventory("large_gear")) {
                gear->set_location("placed");
            }

            return true;
        }

        if (verb == "use" &&
            (object == "medium gear" || object == "medium_gear")) {
            if (!large_gear_placed) {
                std::cout << "You need to place the large gear first." << std::endl;
            }
            else {
                std::cout << "You attach the medium gear to the large one. "
                    << "It meshes perfectly with the teeth of the larger gear." << std::endl;
                medium_gear_placed = true;

                auto gear = get_item("medium_gear");
                if (gear && player.remove_from_inventory("medium_gear")) {
                    gear->set_location("placed");
                }
            }
            return true;
        }

        if (verb == "use" &&
            (object == "small gear" || object == "small_gear")) {
            if (!medium_gear_placed) {
                std::cout << "You need to place the medium gear first." << std::endl;
            }
            else {
                std::cout << "You insert the small gear into the final slot of the mechanism. "
                    << "All the gears now form a complete chain." << std::endl;
                small_gear_placed = true;

                auto gear = get_item("small_gear");
                if (gear && player.remove_from_inventory("small_gear")) {
                    gear->set_location("placed");
                }
            }
            return true;
        }

        if (verb == "activate" &&
            (object == "bridge" || object == "gear bridge" || object == "bridge_repair")) {
            if (large_gear_placed && medium_gear_placed && small_gear_placed) {
                std::cout << "With all gears in place, you activate the mechanism. "
                    << "The bridge extends fully across the chasm with a satisfying series of mechanical clicks." << std::endl;

                std::cout << "As the bridge connects, you spot a Crystal Fragment glinting on the far side." << std::endl;

                bridge_puzzle_solved = true;
                set_game_flag("bridge_puzzle_solved", true);

                auto fragment = get_item("crystal_fragment_1");
                if (fragment) {
                    fragment->set_location("clockwork_forge");
                }
            }
            else {
                std::cout << "The bridge mechanism is still incomplete. You need to place all the gears." << std::endl;
            }
            return true;
        }
    }

    if (current_room_id == "archive_shadows") {
        bool librarian_exists = false;
        for (const auto& npc_ptr : npcs) {
            if (npc_ptr->get_id() == "librarian") {
                librarian_exists = true;
                if (npc_ptr->get_current_room() != "archive_shadows") {
                    npc_ptr->set_current_room("archive_shadows");
                }
                break;
            }
        }

        if (!librarian_exists) {
            auto librarian = std::make_shared<npc>("librarian");
            librarian->set_name("The Librarian");
            librarian->set_description("A spectral entity in the Archive of Shadows");
            librarian->set_role("Knowledge Keeper");
            librarian->set_current_room("archive_shadows");
            add_npc(librarian);
        }

        auto tome = get_item("ancient_tome");
        if (!tome) {
            auto new_tome = std::make_shared<item>("ancient_tome");
            new_tome->set_name("Ancient Tome");
            new_tome->set_description("Contains cryptic knowledge about the Echo Crystal");
            new_tome->set_type("book");
            new_tome->set_property("readable", "true");
            new_tome->set_property("contents", "The Echo Crystal was shattered during the Great Cataclysm. Its five fragments were scattered across Aetheria. Only by reuniting them can balance be restored.");
            new_tome->set_location("archive_shadows");
            add_item(new_tome);
        }

        auto fragment2 = get_item("crystal_fragment_2");
        if (!fragment2) {
            auto new_fragment = std::make_shared<item>("crystal_fragment_2");
            new_fragment->set_name("Crystal Fragment 2");
            new_fragment->set_description("A glowing fragment of the Echo Crystal.");
            new_fragment->set_type("quest_item");
            new_fragment->set_location("archive_shadows");
            add_item(new_fragment);
        }
        else if (fragment2->get_location() != "archive_shadows" &&
            fragment2->get_location() != "inventory" &&
            fragment2->get_location() != "echo_chamber") {
            fragment2->set_location("archive_shadows");
        }

        if ((verb == "examine" || verb == "look") &&
            (object == "librarian" || object == "the librarian")) {
            std::cout << "A ghostly figure drifts among the bookshelves. Its form shifts and wavers, "
                << "but two piercing eyes remain constant, studying you with ancient wisdom." << std::endl;
            return true;
        }

        if (verb == "talk" &&
            (object == "librarian" || object == "the librarian")) {
            std::cout << "The Librarian: \"Knowledge has a price, seeker. Bring me the Ancient Tome, and I shall share what I know.\"\n\n";
            std::cout << "What do you say?\n";
            std::cout << "1: I'll find the tome for you.\n";
            std::cout << "2: What knowledge do you possess?\n";

            std::string choice;
            std::cout << "> ";
            std::getline(std::cin, choice);

            if (choice == "1") {
                std::cout << "The Librarian: \"The tome rests among these shelves. Seek and you shall find.\"\n";
            }
            else if (choice == "2") {
                std::cout << "The Librarian: \"I hold the secret history of Aetheria and the Echo Crystal. But such knowledge is not freely given.\"\n";
            }

            bool has_tome = false;
            for (const auto& item_ptr : player.get_inventory()) {
                if (item_ptr->get_id() == "ancient_tome") {
                    has_tome = true;
                    break;
                }
            }

            if (has_tome) {
                std::cout << "\nThe Librarian notices the Ancient Tome in your possession.\n";
                std::cout << "The Librarian: \"Ah, you have brought the tome. As promised, I shall reveal what I know.\"\n";
                std::cout << "The Librarian tells you about the locations of the Crystal Fragments and the history of the Echo Crystal.\n";
                std::cout << "The Librarian: \"Take this fragment as a token of our exchange. The others await in Ember Peaks, Abyssal Trench, and Veyra's Airship.\"\n";
            }

            return true;
        }

        if ((verb == "examine" || verb == "look") &&
            (object == "book" || object == "tome" || object == "ancient tome")) {
            std::cout << "A weathered tome bound in strange material. Ancient runes decorate its cover, "
                << "and it seems to emanate a subtle glow." << std::endl;
            return true;
        }

        if ((verb == "examine" || verb == "look") &&
            (object == "bookshelves" || object == "shelves" || object == "books")) {
            std::cout << "Rows upon rows of ancient tomes line the shelves. Among them, you notice a particularly "
                << "ornate book that seems to be glowing faintly." << std::endl;
            return true;
        }
    }

    if (current_room_id == "ember_peaks") {
        bool gorath_exists = false;
        for (const auto& npc_ptr : npcs) {
            if (npc_ptr->get_id() == "gorath") {
                gorath_exists = true;
                if (npc_ptr->get_current_room() != "ember_peaks") {
                    npc_ptr->set_current_room("ember_peaks");
                }
                break;
            }
        }

        if (!gorath_exists) {
            auto gorath = std::make_shared<npc>("gorath");
            gorath->set_name("Gorath");
            gorath->set_description("A cursed knight trapped in enchanted armor");
            gorath->set_role("Cursed Knight");
            gorath->set_current_room("ember_peaks");
            add_npc(gorath);
        }

        auto fragment3 = get_item("crystal_fragment_3");
        if (!fragment3) {
            auto new_fragment = std::make_shared<item>("crystal_fragment_3");
            new_fragment->set_name("Crystal Fragment 3");
            new_fragment->set_description("A glowing fragment of the Echo Crystal.");
            new_fragment->set_type("quest_item");
            new_fragment->set_location("ember_peaks");
            add_item(new_fragment);
        }
        else if (fragment3->get_location() != "ember_peaks" &&
            fragment3->get_location() != "inventory" &&
            fragment3->get_location() != "echo_chamber") {
            fragment3->set_location("ember_peaks");
        }

        static bool riddle_solved = false;
        if (get_game_flag("gorath_riddle_solved")) {
            riddle_solved = true;
        }

        if (verb == "talk" &&
            (object == "gorath" || object == "knight")) {
            if (riddle_solved) {
                std::cout << "Gorath: \"You have proven worthy of the crystal's power. Use it wisely.\"\n";
            }
            else {
                std::cout << "Gorath: \"Answer my riddle or face me in combat.\"\n";
                std::cout << "Gorath: \"I am not alive, but I grow; I don't have lungs, but I need air; ";
                std::cout << "I don't have a mouth, but water kills me. What am I?\"\n\n";
                std::cout << "What is your answer?\n";

                std::string answer;
                std::cout << "> ";
                std::getline(std::cin, answer);

                std::string lower_answer = answer;
                std::transform(lower_answer.begin(), lower_answer.end(), lower_answer.begin(),
                    [](unsigned char c) { return std::tolower(c); });

                if (lower_answer == "fire") {
                    std::cout << "Gorath: \"Correct! You have proven your wisdom.\"\n";
                    std::cout << "Gorath presents you with the Crystal Fragment as promised.\n";
                    riddle_solved = true;
                    set_game_flag("gorath_riddle_solved", true);
                }
                else {
                    std::cout << "Gorath: \"Incorrect. Try again when you have discovered the answer.\"\n";
                }
            }
            return true;
        }

        if ((verb == "fire" || verb == "answer") && object.empty()) {
            if (!riddle_solved) {
                std::cout << "Gorath: \"Correct! You have proven your wisdom.\"\n";
                std::cout << "Gorath presents you with the Crystal Fragment as promised.\n";
                riddle_solved = true;
                set_game_flag("gorath_riddle_solved", true);
            }
            else {
                std::cout << "Gorath has already given you the Crystal Fragment.\n";
            }
            return true;
        }

        if (verb == "answer" && to_lower(object) == "fire") {
            if (!riddle_solved) {
                std::cout << "Gorath: \"Correct! You have proven your wisdom.\"\n";
                std::cout << "Gorath presents you with the Crystal Fragment as promised.\n";
                riddle_solved = true;
                set_game_flag("gorath_riddle_solved", true);
            }
            else {
                std::cout << "Gorath has already given you the Crystal Fragment.\n";
            }
            return true;
        }
    }

    if (current_room_id == "skyward_nexus") {
        static bool paths_aligned = false;

        if (get_game_flag("paths_aligned")) {
            paths_aligned = true;
        }

        if ((verb == "examine" || verb == "look") &&
            (object == "floating paths" || object == "paths" || object == "floating_paths")) {
            if (paths_aligned) {
                std::cout << "The floating pathways now form a stable network, allowing access to all the islands." << std::endl;
            }
            else {
                std::cout << "Translucent pathways float in the air, connecting to different islands. "
                    << "They seem to shift and waver, making some destinations difficult to reach." << std::endl;
            }
            return true;
        }

        auto amulet = get_item("echo_amulet");
        if (!amulet) {
            auto new_amulet = std::make_shared<item>("echo_amulet");
            new_amulet->set_name("Echo Amulet");
            new_amulet->set_description("Allows glimpses into past events");
            new_amulet->set_type("artifact");
            new_amulet->set_location("skyward_nexus");
            add_item(new_amulet);
        }

        if (verb == "use" &&
            (object == "echo amulet" || object == "amulet" || object == "echo_amulet")) {
            std::cout << "The amulet glows with an inner light. Ghostly images of the past appear, "
                << "showing how the pathways were originally arranged." << std::endl;
            return true;
        }

        if (verb == "activate" &&
            (object == "path alignment" || object == "paths" || object == "path_alignment")) {
            bool has_amulet = false;
            for (const auto& item_ptr : player.get_inventory()) {
                if (item_ptr->get_id() == "echo_amulet") {
                    has_amulet = true;
                    break;
                }
            }

            if (has_amulet || amulet->get_location() == "inventory") {
                std::cout << "Using the Echo Amulet's visions as a guide, you realign the floating paths. "
                    << "The pathways solidify into a stable network, allowing access to all islands." << std::endl;
                paths_aligned = true;
                set_game_flag("paths_aligned", true);
            }
            else {
                std::cout << "You concentrate on aligning the floating paths. After some trial and error, "
                    << "the pathways solidify into a stable network, allowing access to all islands." << std::endl;
                paths_aligned = true;
                set_game_flag("paths_aligned", true);
            }

            auto nexus = get_room("skyward_nexus");
            if (nexus) {
                nexus->unlock_connection("north");
                nexus->unlock_connection("south");
                nexus->unlock_connection("east");
                nexus->unlock_connection("west");
                nexus->unlock_connection("up");
                nexus->unlock_connection("down");
            }

            return true;
        }

        if (verb == "take" &&
            (object == "echo amulet" || object == "amulet" || object == "echo_amulet")) {

            auto amulet = get_item("echo_amulet");
            if (!amulet) {
                auto new_amulet = std::make_shared<item>("echo_amulet");
                new_amulet->set_name("Echo Amulet");
                new_amulet->set_description("Allows glimpses into past events");
                new_amulet->set_type("artifact");
                new_amulet->set_location(current_room_id);
                add_item(new_amulet);
                amulet = new_amulet;
            }
            else if (amulet->get_location() != "skyward_nexus" &&
                amulet->get_location() != "inventory") {
                amulet->set_location("skyward_nexus");
            }

            if (player.add_to_inventory(amulet)) {
                std::cout << "Taken." << std::endl;
                return true;
            }
            else {
                std::cout << "You can't carry any more items." << std::endl;
                return false;
            }
        }
    }

    if (current_room_id == "veyras_airship") {
        bool veyra_exists = false;
        for (const auto& npc_ptr : npcs) {
            if (npc_ptr->get_id() == "veyra") {
                veyra_exists = true;
                if (npc_ptr->get_current_room() != "veyras_airship") {
                    npc_ptr->set_current_room("veyras_airship");
                }
                break;
            }
        }

        if (!veyra_exists) {
            auto veyra = std::make_shared<npc>("veyra");
            veyra->set_name("Veyra");
            veyra->set_description("A rogue inventor seeking the Echo Crystal to power her airship");
            veyra->set_role("Rogue Inventor");
            veyra->set_current_room("veyras_airship");
            add_npc(veyra);
        }

        auto fragment5 = get_item("crystal_fragment_5");
        if (!fragment5) {
            auto new_fragment = std::make_shared<item>("crystal_fragment_5");
            new_fragment->set_name("Crystal Fragment 5");
            new_fragment->set_description("A glowing fragment of the Echo Crystal.");
            new_fragment->set_type("quest_item");
            new_fragment->set_location("veyras_airship");
            add_item(new_fragment);
        }
        else if (fragment5->get_location() != "veyras_airship" &&
            fragment5->get_location() != "inventory" &&
            fragment5->get_location() != "echo_chamber") {
            fragment5->set_location("veyras_airship");
        }

        if ((verb == "examine" || verb == "look") &&
            (object == "veyra" || object == "inventor")) {
            std::cout << "A sharp-eyed woman dressed in gear-laden attire. Various tools hang from her belt, "
                << "and she studies you with a calculating gaze." << std::endl;
            return true;
        }

        if (verb == "talk" &&
            (object == "veyra" || object == "inventor")) {
            std::cout << "Veyra: \"Perhaps we can help each other, stranger. I need Crystal fragments for my research.\"\n\n";
            std::cout << "What do you say?\n";
            std::cout << "1: What research are you conducting?\n";
            std::cout << "2: I'm collecting the fragments myself.\n";

            std::string choice;
            std::cout << "> ";
            std::getline(std::cin, choice);

            if (choice == "1") {
                std::cout << "Veyra: \"I'm studying how to harness the Crystal's energy for my airship. ";
                std::cout << "The technology could revolutionize travel across the shattered isles.\"\n";
            }
            else if (choice == "2") {
                std::cout << "Veyra: \"I see. Well, perhaps we can still aid each other. I'll let you take the fragment here ";
                std::cout << "if you promise to share what you learn about the Crystal.\"\n";
            }

            set_game_flag("veyra_negotiation_complete", true);
            return true;
        }
    }

    if (current_room_id == "abyssal_trench") {
        static bool pressure_stabilized = false;

        if (get_game_flag("pressure_puzzle_solved")) {
            pressure_stabilized = true;
        }

        auto fragment4 = get_item("crystal_fragment_4");
        if (!fragment4) {
            auto new_fragment = std::make_shared<item>("crystal_fragment_4");
            new_fragment->set_name("Crystal Fragment 4");
            new_fragment->set_description("A glowing fragment of the Echo Crystal.");
            new_fragment->set_type("quest_item");
            new_fragment->set_location("abyssal_trench");
            add_item(new_fragment);
        }
        else if (fragment4->get_location() != "abyssal_trench" &&
            fragment4->get_location() != "inventory" &&
            fragment4->get_location() != "echo_chamber") {
            fragment4->set_location("abyssal_trench");
        }

        auto gauge = get_item("pressure_gauge");
        if (!gauge) {
            auto new_gauge = std::make_shared<item>("pressure_gauge");
            new_gauge->set_name("Pressure Gauge");
            new_gauge->set_description("A device for measuring underwater pressure");
            new_gauge->set_type("tool");
            new_gauge->set_location("abyssal_trench");
            add_item(new_gauge);
        }

        if ((verb == "examine" || verb == "look") &&
            (object == "water spirit" || object == "spirit")) {
            std::cout << "A shimmering presence made of pure water. It moves gracefully through the depths, "
                << "occasionally forming a face to observe you." << std::endl;
            return true;
        }

        if (verb == "use" &&
            (object == "pressure gauge" || object == "gauge" || object == "pressure_gauge")) {
            std::cout << "You use the pressure gauge to measure the water pressure at different depths. "
                << "The readings reveal a pattern that could be used to stabilize the currents." << std::endl;
            return true;
        }

        if (verb == "activate" &&
            (object == "pressure control" || object == "pressure_control")) {
            bool has_gauge = false;
            for (const auto& item_ptr : player.get_inventory()) {
                if (item_ptr->get_id() == "pressure_gauge") {
                    has_gauge = true;
                    break;
                }
            }

            if (has_gauge || gauge->get_location() == "inventory") {
                std::cout << "Using the pressure gauge readings, you adjust the ancient mechanism. "
                    << "The water currents stabilize, revealing a hidden chamber containing the Crystal Fragment." << std::endl;
                pressure_stabilized = true;
                set_game_flag("pressure_puzzle_solved", true);
            }
            else {
                std::cout << "You adjust various controls on the ancient mechanism. By luck or intuition, "
                    << "the water currents stabilize, revealing a hidden chamber containing the Crystal Fragment." << std::endl;
                pressure_stabilized = true;
                set_game_flag("pressure_puzzle_solved", true);
            }
            return true;
        }
    }

    if (current_room_id == "echo_chamber") {
        static bool fragments_combined = false;

        if (get_game_flag("crystal_restored")) {
            fragments_combined = true;
        }

        bool architect_exists = false;
        for (const auto& npc_ptr : npcs) {
            if (npc_ptr->get_id() == "architect") {
                architect_exists = true;
                if (npc_ptr->get_current_room() != "echo_chamber") {
                    npc_ptr->set_current_room("echo_chamber");
                }
                break;
            }
        }

        if (!architect_exists) {
            auto architect = std::make_shared<npc>("architect");
            architect->set_name("The Architect");
            architect->set_description("A mysterious figure who appears in visions");
            architect->set_role("Mysterious Figure");
            architect->set_current_room("echo_chamber");
            add_npc(architect);
        }

        if ((verb == "examine" || verb == "look") &&
            (object == "crystal altar" || object == "altar")) {
            std::cout << "A translucent altar floats at the center of the chamber. Five indentations are visible, "
                << "perfectly shaped to hold the Crystal Fragments." << std::endl;
            return true;
        }

        if (verb == "use" &&
            (object.find("crystal fragment") != std::string::npos ||
                object.find("crystal_fragment") != std::string::npos)) {
            bool has_fragment1 = false;
            bool has_fragment2 = false;
            bool has_fragment3 = false;
            bool has_fragment4 = false;
            bool has_fragment5 = false;

            for (const auto& item_ptr : player.get_inventory()) {
                std::string id = item_ptr->get_id();
                if (id == "crystal_fragment_1") has_fragment1 = true;
                else if (id == "crystal_fragment_2") has_fragment2 = true;
                else if (id == "crystal_fragment_3") has_fragment3 = true;
                else if (id == "crystal_fragment_4") has_fragment4 = true;
                else if (id == "crystal_fragment_5") has_fragment5 = true;
            }

            int fragment_count = 0;
            if (has_fragment1) fragment_count++;
            if (has_fragment2) fragment_count++;
            if (has_fragment3) fragment_count++;
            if (has_fragment4) fragment_count++;
            if (has_fragment5) fragment_count++;

            if (fragment_count >= 3) {
                std::cout << "You place all your Crystal Fragments on the altar. They begin to glow intensely, "
                    << "rising into the air and drawing together. With a flash of light, they merge into the complete Echo Crystal." << std::endl;
                fragments_combined = true;
                set_game_flag("crystal_restored", true);

                auto crystal = std::make_shared<item>("echo_crystal");
                crystal->set_name("Echo Crystal");
                crystal->set_description("The restored Echo Crystal, pulsing with otherworldly power.");
                crystal->set_type("artifact");
                crystal->set_location("echo_chamber");
                add_item(crystal);

                if (has_fragment1) player.remove_from_inventory("crystal_fragment_1");
                if (has_fragment2) player.remove_from_inventory("crystal_fragment_2");
                if (has_fragment3) player.remove_from_inventory("crystal_fragment_3");
                if (has_fragment4) player.remove_from_inventory("crystal_fragment_4");
                if (has_fragment5) player.remove_from_inventory("crystal_fragment_5");
            }
            else {
                std::cout << "You place the fragment on the altar, but nothing happens. "
                    << "It seems you need more fragments to restore the Crystal." << std::endl;
            }
            return true;
        }

        if (verb == "talk" &&
            (object == "architect" || object == "the architect")) {
            if (fragments_combined) {
                std::cout << "The Architect: \"You must choose the fate of Aetheria.\"\n\n";
                std::cout << "The Architect presents you with three choices:\n";
                std::cout << "1: Restore balance and sacrifice yourself\n";
                std::cout << "2: Seize power and reshape reality\n";
                std::cout << "3: Shatter the crystal and end the cycle\n";

                std::string choice;
                std::cout << "> ";
                std::getline(std::cin, choice);

                if (choice == "1") {
                    std::cout << "\nYou channel the Crystal's power, sacrificing your own existence to restore Aetheria.\n";
                    std::cout << "The shattered islands begin to rejoin, and balance returns to the world.\n";
                    std::cout << "Though you cease to exist in this timeline, your legacy lives on in the restored realm.\n";
                    std::cout << "\n*** THE END - RESTORATION ENDING ***\n";
                }
                else if (choice == "2") {
                    std::cout << "\nYou absorb the Crystal's power, becoming a godlike entity.\n";
                    std::cout << "Reality bends to your will as you reshape Aetheria according to your vision.\n";
                    std::cout << "But with such power comes consequences that even you cannot foresee...\n";
                    std::cout << "\n*** THE END - DOMINATION ENDING ***\n";
                }
                else if (choice == "3") {
                    std::cout << "\nYou shatter the newly-restored Crystal, breaking the cycle permanently.\n";
                    std::cout << "The fragments dissolve into pure energy, dispersing throughout Aetheria.\n";
                    std::cout << "The world will never be whole again, but neither will it be bound by ancient powers.\n";
                    std::cout << "\n*** THE END - OBLIVION ENDING ***\n";
                }
            }
            else {
                int fragment_count = 0;
                for (int i = 1; i <= 5; i++) {
                    if (player.get_item_from_inventory("crystal_fragment_" + std::to_string(i))) {
                        fragment_count++;
                    }
                }

                if (fragment_count >= 3) {
                    std::cout << "The Architect: \"You have the fragments. Place them on the altar to restore the Crystal.\"\n";
                }
                else {
                    std::cout << "The Architect: \"The Crystal remains incomplete. Gather more fragments from across Aetheria and place them on the altar.\"\n";
                }
            }
            return true;
        }
    }

    if ((verb == "examine" || verb == "look") && !object.empty()) {
        const auto& features = current_room->get_features();
        for (const auto& feature : features) {
            std::string feature_lower = feature;
            std::transform(feature_lower.begin(), feature_lower.end(), feature_lower.begin(),
                [](unsigned char c) { return std::tolower(c); });

            std::string object_lower = object;
            std::transform(object_lower.begin(), object_lower.end(), object_lower.begin(),
                [](unsigned char c) { return std::tolower(c); });

            if (feature_lower == object_lower ||
                feature_lower.find(object_lower) != std::string::npos ||
                object_lower.find(feature_lower) != std::string::npos) {

                std::cout << "You examine the " << feature << " closely, but don't notice anything special." << std::endl;
                return true;
            }
        }
    }

    const auto& puzzles = current_room->get_puzzles();
    for (const auto& puzzle : puzzles) {
        if (puzzle.command == verb && (object.empty() || puzzle.object == object)) {
            if (puzzle.solved) {
                std::cout << "You've already solved this puzzle." << std::endl;
                return true;
            }

            bool has_all_items = true;
            for (const auto& req_item : puzzle.required_items) {
                bool found = false;
                for (const auto& player_item : player.get_inventory()) {
                    if (player_item->get_id() == req_item) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    has_all_items = false;
                    break;
                }
            }

            if (!has_all_items) {
                std::cout << "You don't have the necessary items to do that." << std::endl;
                return true;
            }

            std::cout << puzzle.success_message << std::endl;

            current_room->solve_puzzle(puzzle.id);
            if (!puzzle.reward_item.empty()) {
                auto reward = get_item(puzzle.reward_item);
                if (reward) {
                    reward->set_location(player.get_current_room());
                    std::cout << "Your actions have revealed " << reward->get_name() << "!" << std::endl;
                }
            }

            if (!puzzle.unlocks_path.empty()) {
                current_room->unlock_connection(puzzle.unlocks_path);
                std::cout << "You've unlocked a new path!" << std::endl;
            }

            if (!puzzle.sets_flag.empty()) {
                set_game_flag(puzzle.sets_flag, true);
            }

            return true;
        }
    }

    return false;
}

void world::ensure_npcs_in_proper_locations() {
    for (auto& npc_ptr : npcs) {
        if (!npc_ptr) continue;

        std::string npc_id = npc_ptr->get_id();
        std::string proper_location;

        if (npc_id == "guardian_automaton") {
            proper_location = "sanctum_whispers";
        }
        else if (npc_id == "librarian") {
            proper_location = "archive_shadows";
        }
        else if (npc_id == "gorath") {
            proper_location = "ember_peaks";
        }
        else if (npc_id == "veyra") {
            proper_location = "veyras_airship";
        }
        else if (npc_id == "architect") {
            proper_location = "echo_chamber";
        }

        if (!proper_location.empty() && npc_ptr->get_current_room() != proper_location) {
            npc_ptr->set_current_room(proper_location);
        }
    }
}