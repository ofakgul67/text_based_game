#include "game_engine.hpp"
#include "../json_loader/json_loader.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cctype>

game_engine::game_engine() : game_running(true), config_path("game_config.json") {}

void game_engine::initialize() {
    json_loader loader;
    bool loaded = loader.load_game_data(config_path, game_world);

    if (!loaded) {
        std::cerr << "Failed to load game data. Creating game world manually." << std::endl;

        game_world.set_world_name("The Labyrinth of Echoes");
        game_world.set_world_description("A fractured realm where ancient magic and steampunk technology coexist. Centuries ago, a cataclysmic event shattered the world into floating islands, each holding remnants of lost civilizations.");

        auto sanctum = std::make_shared<room>("sanctum_whispers");
        sanctum->set_name("Sanctum of Whispers");
        sanctum->set_type("starting_area");
        sanctum->set_short_description("The ancient sanctum");
        sanctum->set_long_description("Ancient stone walls covered in glowing runes surround you. Mechanical guardians stand motionless in their alcoves, their crystal eyes dimly pulsing.");
        sanctum->add_feature("glowing_runes");
        sanctum->add_feature("ancient_altar");
        sanctum->add_feature("automaton_guardians");
        sanctum->add_feature("western_wall");
        sanctum->add_connection("north", "clockwork_forge", "clockwork_key");
        sanctum->add_connection("east", "archive_shadows");
        game_world.add_room(sanctum);

        auto forge = std::make_shared<room>("clockwork_forge");
        forge->set_name("Clockwork Forge");
        forge->set_type("puzzle_area");
        forge->set_short_description("The mechanical forge");
        forge->set_long_description("Enormous gears turn slowly overhead, driving countless smaller mechanisms. Steam hisses from copper pipes, and the air thrums with mechanical energy.");
        forge->add_feature("gear_bridge");
        forge->add_feature("steam_vents");
        forge->add_feature("mechanical_workbench");
        forge->add_connection("south", "sanctum_whispers");
        forge->add_connection("east", "skyward_nexus");
        game_world.add_room(forge);

        auto archive = std::make_shared<room>("archive_shadows");
        archive->set_name("Archive of Shadows");
        archive->set_type("knowledge_area");
        archive->set_short_description("The shadowy archive");
        archive->set_long_description("Towering bookshelves fade into darkness above. Ghostly lights drift between the stacks, illuminating ancient tomes and scrolls.");
        archive->add_connection("west", "sanctum_whispers");
        archive->add_connection("north", "skyward_nexus");
        game_world.add_room(archive);

        auto nexus = std::make_shared<room>("skyward_nexus");
        nexus->set_name("Skyward Nexus");
        nexus->set_type("hub_area");
        nexus->set_short_description("The floating nexus");
        nexus->set_long_description("Multiple floating pathways converge here, each leading to a different island. Ancient technology keeps the platform aloft.");
        nexus->add_feature("floating_paths");
        nexus->add_feature("crystal_pylons");
        nexus->add_connection("west", "clockwork_forge");
        nexus->add_connection("south", "archive_shadows");
        nexus->add_connection("north", "veyras_airship");
        nexus->add_connection("down", "abyssal_trench");
        game_world.add_room(nexus);

        auto peaks = std::make_shared<room>("ember_peaks");
        peaks->set_name("Ember Peaks");
        peaks->set_type("combat_area");
        peaks->set_short_description("The burning peaks");
        peaks->set_long_description("Rivers of lava flow between crystalline formations. The air shimmers with heat, and ancient forges glow in the depths.");
        peaks->add_connection("south", "clockwork_forge");
        peaks->add_connection("east", "veyras_airship");
        game_world.add_room(peaks);

        auto airship = std::make_shared<room>("veyras_airship");
        airship->set_name("Veyra's Airship");
        airship->set_type("mechanical_area");
        airship->set_short_description("The crystal airship");
        airship->set_long_description("Brass and copper machinery fills the ship. Steam hisses from pipes, and crystal engines pulse with power.");
        airship->add_connection("south", "skyward_nexus");
        airship->add_connection("west", "ember_peaks");
        game_world.add_room(airship);

        auto trench = std::make_shared<room>("abyssal_trench");
        trench->set_name("Abyssal Trench");
        trench->set_type("underwater_area");
        trench->set_short_description("The dark depths");
        trench->set_long_description("Crystal-clear waters reveal ancient ruins below. Strange creatures dart through the depths, and forgotten treasures glitter in the dark.");
        trench->add_connection("up", "skyward_nexus");
        trench->add_connection("east", "echo_chamber");
        game_world.add_room(trench);

        auto chamber = std::make_shared<room>("echo_chamber");
        chamber->set_name("Echo Chamber");
        chamber->set_type("final_area");
        chamber->set_short_description("The crystal chamber");
        chamber->set_long_description("Reality itself seems to waver here. Fragments of the past play out in ghostly echoes around you.");
        chamber->add_feature("crystal_altar");
        chamber->add_feature("reality_rifts");
        chamber->add_feature("time_echoes");
        chamber->add_connection("west", "abyssal_trench");
        game_world.add_room(chamber);

        auto guardian = std::make_shared<npc>("guardian_automaton");
        guardian->set_name("Guardian Automaton");
        guardian->set_description("A towering mechanical guardian, seemingly inactive.");
        guardian->set_current_room("sanctum_whispers");
        game_world.add_npc(guardian);

        auto librarian = std::make_shared<npc>("librarian");
        librarian->set_name("The Librarian");
        librarian->set_description("A spectral entity in the Archive of Shadows");
        librarian->set_role("Knowledge Keeper");
        librarian->set_current_room("archive_shadows");
        game_world.add_npc(librarian);

        auto gorath = std::make_shared<npc>("gorath");
        gorath->set_name("Gorath");
        gorath->set_description("A cursed knight trapped in enchanted armor");
        gorath->set_role("Cursed Knight");
        gorath->set_current_room("ember_peaks");
        game_world.add_npc(gorath);

        auto veyra = std::make_shared<npc>("veyra");
        veyra->set_name("Veyra");
        veyra->set_description("A rogue inventor seeking the Echo Crystal to power her airship");
        veyra->set_role("Rogue Inventor");
        veyra->set_current_room("veyras_airship");
        game_world.add_npc(veyra);

        auto architect = std::make_shared<npc>("architect");
        architect->set_name("The Architect");
        architect->set_description("A mysterious figure who appears in visions");
        architect->set_role("Mysterious Figure");
        architect->set_current_room("echo_chamber");
        game_world.add_npc(architect);

        auto compass = std::make_shared<item>("runed_compass");
        compass->set_name("Runed Compass");
        compass->set_description("Points toward hidden pathways");
        compass->set_type("tool");
        compass->set_property("reveals_secrets", "true");
        game_world.add_item(compass);

        auto key = std::make_shared<item>("clockwork_key");
        key->set_name("Clockwork Key");
        key->set_description("A brass key used to operate steampunk machinery");
        key->set_type("key");
        game_world.add_item(key);

        auto tome = std::make_shared<item>("ancient_tome");
        tome->set_name("Ancient Tome");
        tome->set_description("Contains cryptic knowledge about the Echo Crystal");
        tome->set_type("book");
        tome->set_property("readable", "true");
        tome->set_property("contents", "The Echo Crystal was shattered during the Great Cataclysm. Its five fragments were scattered across Aetheria. Only by reuniting them can balance be restored.");
        tome->set_location("archive_shadows");
        game_world.add_item(tome);

        auto large_gear = std::make_shared<item>("large_gear");
        large_gear->set_name("Large Gear");
        large_gear->set_description("A hefty metal gear that appears to be part of a mechanism");
        large_gear->set_type("part");
        large_gear->set_location("clockwork_forge");
        game_world.add_item(large_gear);

        auto medium_gear = std::make_shared<item>("medium_gear");
        medium_gear->set_name("Medium Gear");
        medium_gear->set_description("A medium-sized gear with intricate teeth");
        medium_gear->set_type("part");
        medium_gear->set_location("clockwork_forge");
        game_world.add_item(medium_gear);

        auto small_gear = std::make_shared<item>("small_gear");
        small_gear->set_name("Small Gear");
        small_gear->set_description("A small but precisely crafted gear");
        small_gear->set_type("part");
        small_gear->set_location("clockwork_forge");
        game_world.add_item(small_gear);

        auto amulet = std::make_shared<item>("echo_amulet");
        amulet->set_name("Echo Amulet");
        amulet->set_description("Allows glimpses into past events");
        amulet->set_type("artifact");
        amulet->set_location("skyward_nexus");
        game_world.add_item(amulet);

        auto gauge = std::make_shared<item>("pressure_gauge");
        gauge->set_name("Pressure Gauge");
        gauge->set_description("A device for measuring underwater pressure");
        gauge->set_type("tool");
        gauge->set_location("abyssal_trench");
        game_world.add_item(gauge);

        for (int i = 1; i <= 5; i++) {
            auto fragment = std::make_shared<item>("crystal_fragment_" + std::to_string(i));
            fragment->set_name("Crystal Fragment " + std::to_string(i));
            fragment->set_description("A glowing fragment of the Echo Crystal");
            fragment->set_type("quest_item");

            switch (i) {
                case 1: fragment->set_location("clockwork_forge"); break;
                case 2: fragment->set_location("archive_shadows"); break;
                case 3: fragment->set_location("ember_peaks"); break;
                case 4: fragment->set_location("abyssal_trench"); break;
                case 5: fragment->set_location("veyras_airship"); break;
            }

            game_world.add_item(fragment);
        }

        game_world.set_starting_room("sanctum_whispers");
        game_world.add_starting_item("runed_compass");
        game_world.set_player_health(100);
        game_world.set_player_inventory_size(10);
    }

    player_character.set_current_room(game_world.get_starting_room());
    player_character.set_inventory_size(game_world.get_player_inventory_size());

    for (const auto& item_id : game_world.get_starting_inventory()) {
        const auto& item = game_world.get_item(item_id);
        if (item) {
            player_character.add_to_inventory(item);
        }
    }

    player_character.set_health(game_world.get_player_health());
    fix_game_paths_and_fragments();
    print_introduction();
}

void game_engine::run() {
    std::cout << game_world.get_room_description(player_character.get_current_room(), true) << std::endl;

    std::string command;
    while (game_running) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command.empty()) {
            continue;
        }

        process_command(command);
        update_npcs(); 
    }
}

void game_engine::fix_game_paths_and_fragments() {
    auto nexus = game_world.get_room("skyward_nexus");
    if (nexus) {
        nexus->add_connection("north", "veyras_airship");
        nexus->add_connection("south", "archive_shadows");
        nexus->add_connection("west", "clockwork_forge");
        nexus->add_connection("east", "ember_peaks");  
        nexus->add_connection("down", "abyssal_trench");

        nexus->unlock_connection("north");
        nexus->unlock_connection("south");
        nexus->unlock_connection("west");
        nexus->unlock_connection("east");
        nexus->unlock_connection("down");
    }

    auto ember_peaks = game_world.get_room("ember_peaks");
    if (!ember_peaks) {
        ember_peaks = std::make_shared<room>("ember_peaks");
        ember_peaks->set_name("Ember Peaks");
        ember_peaks->set_type("combat_area");
        ember_peaks->set_short_description("The burning peaks");
        ember_peaks->set_long_description("Rivers of lava flow between crystalline formations. The air shimmers with heat, and ancient forges glow in the depths.");
        ember_peaks->add_connection("west", "skyward_nexus");
        game_world.add_room(ember_peaks);
    }
    else {
        ember_peaks->add_connection("west", "skyward_nexus");
    }

    auto abyssal = game_world.get_room("abyssal_trench");
    if (!abyssal) {
        abyssal = std::make_shared<room>("abyssal_trench");
        abyssal->set_name("Abyssal Trench");
        abyssal->set_type("underwater_area");
        abyssal->set_short_description("The dark depths");
        abyssal->set_long_description("Crystal-clear waters reveal ancient ruins below. Strange creatures dart through the depths, and forgotten treasures glitter in the dark.");
        abyssal->add_connection("up", "skyward_nexus");
        abyssal->add_connection("east", "echo_chamber");
        game_world.add_room(abyssal);
    }
    else {
        abyssal->add_connection("up", "skyward_nexus");
        abyssal->add_connection("east", "echo_chamber");
    }

    auto chamber = game_world.get_room("echo_chamber");
    if (!chamber) {
        chamber = std::make_shared<room>("echo_chamber");
        chamber->set_name("Echo Chamber");
        chamber->set_type("final_area");
        chamber->set_short_description("The crystal chamber");
        chamber->set_long_description("Reality itself seems to waver here. Fragments of the past play out in ghostly echoes around you.");
        chamber->add_feature("crystal_altar");
        chamber->add_feature("reality_rifts");
        chamber->add_connection("west", "abyssal_trench");
        game_world.add_room(chamber);
    }
    else {
        chamber->add_connection("west", "abyssal_trench");
    }

    bool architect_exists = false;
    for (auto& npc : game_world.get_npcs()) {
        if (npc->get_id() == "architect") {
            npc->set_current_room("echo_chamber");
            architect_exists = true;
            break;
        }
    }

    if (!architect_exists) {
        auto architect = std::make_shared<npc>("architect");
        architect->set_name("The Architect");
        architect->set_description("A mysterious figure who appears in visions");
        architect->set_role("Mysterious Figure");
        architect->set_current_room("echo_chamber");
        game_world.add_npc(architect);
    }

    auto fragment3 = game_world.get_item("crystal_fragment_3");
    if (!fragment3) {
        auto new_fragment = std::make_shared<item>("crystal_fragment_3");
        new_fragment->set_name("Crystal Fragment 3");
        new_fragment->set_description("A glowing fragment of the Echo Crystal.");
        new_fragment->set_type("quest_item");
        new_fragment->set_location("ember_peaks");
        game_world.add_item(new_fragment);
    }
    else if (fragment3->get_location() != "inventory") {
        fragment3->set_location("ember_peaks");
    }

    auto fragment4 = game_world.get_item("crystal_fragment_4");
    if (!fragment4) {
        auto new_fragment = std::make_shared<item>("crystal_fragment_4");
        new_fragment->set_name("Crystal Fragment 4");
        new_fragment->set_description("A glowing fragment of the Echo Crystal.");
        new_fragment->set_type("quest_item");
        new_fragment->set_location("abyssal_trench");
        game_world.add_item(new_fragment);
    }
    else if (fragment4->get_location() != "inventory") {
        fragment4->set_location("abyssal_trench");
    }
}

void game_engine::hide_fragments_until_puzzles_solved() {
    auto fragment1 = game_world.get_item("crystal_fragment_1");
    if (fragment1) {
        fragment1->set_location("hidden");
        game_world.set_game_flag("bridge_puzzle_solved", false);
    }

    auto fragment2 = game_world.get_item("crystal_fragment_2");
    if (fragment2) {
        fragment2->set_location("hidden");
        game_world.set_game_flag("librarian_puzzle_solved", false);
    }

    auto fragment3 = game_world.get_item("crystal_fragment_3");
    if (fragment3) {
        fragment3->set_location("hidden");
        game_world.set_game_flag("gorath_riddle_solved", false);
    }

    auto fragment4 = game_world.get_item("crystal_fragment_4");
    if (fragment4) {
        fragment4->set_location("hidden");
        game_world.set_game_flag("pressure_puzzle_solved", false);
    }

    auto fragment5 = game_world.get_item("crystal_fragment_5");
    if (fragment5) {
        fragment5->set_location("hidden");
        game_world.set_game_flag("veyra_negotiation_complete", false);
    }
}

void game_engine::process_command(const std::string& command) {
    std::string lower_command = command;
    std::transform(lower_command.begin(), lower_command.end(), lower_command.begin(),
        [](unsigned char c) { return std::tolower(c); });

    if (lower_command == "quit" || lower_command == "exit") {
        std::cout << "Are you sure you want to quit? (y/n): ";
        std::string confirm;
        std::getline(std::cin, confirm);
        if (confirm == "y" || confirm == "Y") {
            game_running = false;
        }
        return;
    }
    else if (lower_command == "help") {
        print_help();
        return;
    }
    else if (lower_command == "save") {
        std::cout << "Enter save file name: ";
        std::string filename;
        std::getline(std::cin, filename);
        if (!filename.empty()) {
            save_game(filename);
        }
        return;
    }
    else if (lower_command == "load") {
        std::cout << "Enter save file name to load: ";
        std::string filename;
        std::getline(std::cin, filename);
        if (!filename.empty()) {
            load_game(filename);
        }
        return;
    }
    else if (lower_command == "look") {
        std::cout << game_world.get_room_description(player_character.get_current_room(), true) << std::endl;
        return;
    }
    else if (lower_command == "inventory" || lower_command == "i") {
        player_character.display_inventory();
        return;
    }

    std::string verb, object;
    command_parser.parse_command(lower_command, verb, object);

    if (verb == "n" || verb == "north" || verb == "s" || verb == "south" ||
        verb == "e" || verb == "east" || verb == "w" || verb == "west" ||
        verb == "up" || verb == "down") {
        std::string direction;
        if (verb == "n" || verb == "north") direction = "north";
        else if (verb == "s" || verb == "south") direction = "south";
        else if (verb == "e" || verb == "east") direction = "east";
        else if (verb == "w" || verb == "west") direction = "west";
        else if (verb == "up") direction = "up";
        else if (verb == "down") direction = "down";

        game_world.move_player(player_character, direction);
        return;
    }

    bool success = command_parser.execute_command(verb, object, player_character, game_world);
    if (!success) {
        if (object.empty()) {
            std::cout << "Sorry, I don't know the verb \"" << verb << "\"." << std::endl;
        }
        else {
            std::cout << "I don't understand \"" << verb << " " << object << "\"." << std::endl;
        }
    }
}

void game_engine::print_help() const {
    std::cout << "Available commands:\n";
    std::cout << "  Movement: north/n, south/s, east/e, west/w, up, down\n";
    std::cout << "  Actions: look, inventory/i, take [item], drop [item], use [item], examine [item/npc]\n";
    std::cout << "           talk [npc], read [item], activate [item]\n";
    std::cout << "  Game: help, save, load, quit/exit\n";
}

void game_engine::print_introduction() const {
    std::cout << "=================================================\n";
    std::cout << "           THE LABYRINTH OF ECHOES               \n";
    std::cout << "=================================================\n\n";

    std::cout << game_world.get_world_description() << "\n\n";

    std::cout << "You wake up in the Sanctum of Whispers with no memory of your past.\n";
    std::cout << "Your only guide is a strange runed compass that seems to pull you forward.\n";
    std::cout << "Something tells you that the Echo Crystal is the key to your forgotten identity...\n\n";
}

void game_engine::update_npcs() {
    game_world.update_npcs(player_character);
}

void game_engine::save_game(const std::string& filename) const {
    std::ofstream out_file(filename);
    if (!out_file) {
        std::cout << "Error: Could not create save file." << std::endl;
        return;
    }

    out_file << "PLAYER\n";
    out_file << player_character.get_current_room() << "\n";
    out_file << player_character.get_health() << "\n";

    out_file << "INVENTORY\n";
    const auto& inventory = player_character.get_inventory();
    out_file << inventory.size() << "\n";
    for (const auto& item : inventory) {
        out_file << item->get_id() << "\n";
    }

    out_file << "FLAGS\n";
    const auto& flags = game_world.get_game_flags();
    out_file << flags.size() << "\n";
    for (const auto& flag_pair : flags) {
        out_file << flag_pair.first << " " << flag_pair.second << "\n";
    }

    out_file << "NPCS\n";
    const auto& npcs = game_world.get_npcs();
    out_file << npcs.size() << "\n";
    for (const auto& npc : npcs) {
        out_file << npc->get_id() << " " << npc->get_current_room() << " " << npc->get_state() << "\n";
    }

    std::cout << "Game saved to " << filename << "." << std::endl;
}

void game_engine::load_game(const std::string& filename) {
    std::ifstream in_file(filename);
    if (!in_file) {
        std::cout << "Error: Could not open save file." << std::endl;
        return;
    }

    std::string line;

    std::getline(in_file, line); 

    std::getline(in_file, line);
    std::string room_id = line;
    player_character.set_current_room(room_id);

    std::getline(in_file, line);
    int health = std::stoi(line);
    player_character.set_health(health);

    std::getline(in_file, line); 
    std::getline(in_file, line);
    int inv_size = std::stoi(line);

    player_character.clear_inventory();
    for (int i = 0; i < inv_size; i++) {
        std::getline(in_file, line);
        std::string item_id = line;
        const auto& item = game_world.get_item(item_id);
        if (item) {
            player_character.add_to_inventory(item);
        }
    }

    std::getline(in_file, line); 
    std::getline(in_file, line);
    int flags_count = std::stoi(line);

    for (int i = 0; i < flags_count; i++) {
        std::getline(in_file, line);
        std::istringstream iss(line);
        std::string flag_name;
        bool flag_value;
        iss >> flag_name >> flag_value;
        game_world.set_game_flag(flag_name, flag_value);
    }

    std::getline(in_file, line); 
    std::getline(in_file, line);
    int npc_count = std::stoi(line);

    for (int i = 0; i < npc_count; i++) {
        std::getline(in_file, line);
        std::istringstream iss(line);
        std::string npc_id, npc_room, npc_state;
        iss >> npc_id >> npc_room >> npc_state;
        game_world.update_npc_state(npc_id, npc_room, npc_state);
    }

    std::cout << "Game loaded from " << filename << "." << std::endl;
    std::cout << game_world.get_room_description(player_character.get_current_room(), true) << std::endl;
}
