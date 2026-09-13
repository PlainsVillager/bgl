//
// Created by littl on 2026/9/1.
//
#include "Launcher.h"
#include "Utility.h"
#include "configuration/Player.hpp"
#include "configuration/PlayerManager.hpp"
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {
void printWelcome()
{
    std::cout << "Bgl Minecraft Launcher Console Version " << bgl::constants::LAUNCHER_VER_MAJOR_STR << '.' << bgl::constants::LAUNCHER_VER_MINOR_STR << '\n';
}

void helpAction(std::string_view queryCmd)
{
    if (queryCmd == "about") {
        std::cout << "About this program" << '\n';
    } else if (queryCmd == "download") {
        std::cout << "Download a Minecraft instance from internet\n"
                  << "command syntax: download <name: string>\n"
                  << "param explanation: \"name\" for e.g. 26.2 or 20w06a or 26.3-snapshot-10 with no blank)" << '\n';
    } else if (queryCmd == "exit") {
        std::cout << "Exit program." << '\n';
    } else if (queryCmd == "help") {
        std::cout << "You can't query current command!" << '\n';
    } else if (queryCmd == "list") {
        std::cout << "List local installed instances" << '\n';
    } else if (queryCmd == "launch") {
        std::cout << "Launch a local Minecraft instance\n"
                  << "command syntax: launch <name: string>\n"
                  << "param explanation: \"name\" for e.g. 26.2 or 20w06a or 26.3-snapshot-10 with no blank)" << "param explanation: \"name\" for e.g. 26.2 or 20w06a or 26.3-snapshot-10 with no blank)" << '\n';
    } else if (queryCmd == "player") {
        std::cout << "Player related operation\n";
        std::cout << "player add <playerName : string> or player <playerName : string>\n";
    } else if (queryCmd == "refresh") {
        std::cout << "Refresh versions installed\n";
    } else if (queryCmd.empty()) {
        std::cout << "about\ndownload\nexit\nhelp\nlist\nlaunch\nplayer\nrefresh\nremove\nupdate\n";
    } else {
        std::cout << "Unknown command. Retry later" << '\n';
    }
}

void aboutAction()
{
    std::cout << "A cli Minecraft Java Edition Launcher by PlainsVillager" << '\n';
}

void shutDownAction()
{
    std::cout << "Shutting down." << '\n';
    std::exit(0);
}

void refreshInstanceAction()
{
    bgl::Launcher::getSingleton().scanInstance();
}

void downloadAction(const std::string& version)
{
    if (version.empty()) {
        std::cout << "Please input a mc version name\n";
        return;
    }
    {
        auto& instances = bgl::Launcher::getSingleton().getInstances();
        for (const auto& instance : instances) {
            if (instance->getName() == version) {
                std::cout << "This version has been installed. So we will be checking file hash soon.\n";
            }
        }
    }

    bgl::Instance currentInst { version };
    if (!currentInst.downloadAndVerify()) {
        std::cout << std::format("Failed to download {}\n", version);
    }
}

void listAction()
{
    auto& instances = bgl::Launcher::getSingleton().getInstances();

    if (instances.empty()) {
        std::cout << "No instances are installed. Try install by 'download' command.\n";
        return;
    }
    std::cout << "Installed Minecraft instances are list below:\n";
    for (const auto& e : instances) {
        std::cout << e->getName() << '\n';
    }
}

void launchAction(std::string_view instName, const std::string& playerName)
{
    if (instName.empty() || playerName.empty()) {
        std::cout << "Invalid syntax.\n";
        return;
    }

    std::string name, uuid;
    bool flag { false };
    auto& players { bgl::PlayerManager::getPlayerManagerSingleton().listPlayers() };
    for (auto& e : players) {
        if (e.getName() == playerName) {
            flag = true;
            name = e.getName();
            uuid = e.getUuid();
            break;
        }
    }

    if (!flag) {
        std::cout << "Player not found\n";
        return;
    }

    auto& instances = bgl::Launcher::getSingleton().getInstances();
    for (auto& instance : instances) {
        if (instance->getName() == instName) {
            instance->launch(name, uuid);
            break;
        }
    }
}

void playerAction(std::string operation, std::string player_name) // NOLINT
{
    if (operation == "add") {
        if (player_name.empty())
            std::cout << "Invalid syntax\n";
        bgl::PlayerManager::getPlayerManagerSingleton().add(player_name, bgl::generateUUID());
        bgl::PlayerManager::getPlayerManagerSingleton().save();
    } else if (operation == "remove") {
        if (player_name.empty())
            std::cout << "Invalid syntax\n";
        bgl::PlayerManager::getPlayerManagerSingleton().remove(player_name);
        bgl::PlayerManager::getPlayerManagerSingleton().save();
    } else if (operation == "list") {
        auto& vec { bgl::PlayerManager::getPlayerManagerSingleton().listPlayers() };
        for (auto& player : vec) {
            std::cout << player.getName() << ' ' << player.getUuid() << '\n';
        }
    } else {
        std::cout << "Invalid syntax\n";
    }
}

} // namespace

namespace bgl {
Launcher::Launcher() = default;

Launcher& Launcher::getSingleton()
{
    static Launcher singleton { };
    return singleton;
}

void Launcher::startLoop()
{
    printWelcome();

    while (true) {
        getSingleton().scanInstance();
        PlayerManager::getPlayerManagerSingleton().load();
        std::cout << ">>";
        std::string cmd;
        std::getline(std::cin, cmd);

        std::vector<std::string> args { };
        std::istringstream iss { cmd };
        std::string arg;
        while (iss >> arg) {
            args.emplace_back(arg);
        }
        if (args.size() == 1) {
            args.emplace_back();
        } else if (args.size() >= 4) {
            std::cout << "Too many arguments." << '\n';
            continue;
        } else if (arg.empty()) {
            continue;
        }
        // execute
        try {
            if (args[0] == "about") {
                aboutAction();
            } else if (args[0] == "download") {
                downloadAction(args.at(1));
            } else if (args[0] == "exit") {
                shutDownAction();
            } else if (args[0] == "help") {
                helpAction(args.at(1));
            } else if (args[0] == "launch") {
                launchAction(args.at(1), args.at(2));
            } else if (args[0] == "list") {
                listAction();
            } else if (args[0] == "refresh") {
                refreshInstanceAction();
            } else if (args[0] == "player") {
                playerAction(args.at(1), args.at(2));
            }
        } catch (const std::out_of_range& e) {
            std::cerr << "Too less arguments. Please retry.\n";
        }
    }
}

std::vector<std::unique_ptr<Instance>>& Launcher::getInstances()
{
    return instances_;
}

void Launcher::scanInstance()
{
    instances_.clear();
    namespace fs = std::filesystem;
    const fs::path versionsPath = ".minecraft/versions";
    if (!fs::exists(versionsPath))
        fs::create_directories(versionsPath);
    for (const auto& entry : fs::directory_iterator(versionsPath)) {
        if (!entry.is_directory())
            continue;
        auto jar = std::format("{}/client.jar", entry.path().generic_string());
        auto name = getFileName(entry.path().generic_string());
        auto json = std::format("{}/{}.json", entry.path().generic_string(), name);
        if (fs::exists(jar) && fs::exists(json)) {
            instances_.emplace_back(std::make_unique<Instance>(name));
        }
    }
}
} // namespace bgl
