//
// Created by littl on 2026/9/1.
//
#include "Launcher.h"
#include "Utility.h"
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string_view>
#include <unordered_map>
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
    } else if (queryCmd == "refresh") {
        std::cout << "Refresh versions installed\n";
    } else if (queryCmd.empty()) {
        std::cout << "about\ndownload\nexit\nhelp\nlist\nlaunch\nrefresh\nremove\nupdate\n";
    } else {
        std::cout << "Unknown command. Retry later" << '\n';
    }
}

void aboutAction(std::string_view param)
{
    std::cout << "A cli Minecraft Java Edition Launcher by PlainsVillager" << '\n';
}

void shutDownAction(std::string_view param)
{
    std::cout << "Shutting down." << '\n';
    std::exit(0);
}

void refreshInstanceAction(std::string_view param)
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

void listAction(std::string_view param)
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

void launchAction(std::string_view name)
{
    if (name.empty()) {
        std::cout << "a mc version name must be given as the second param.\n";
        return;
    }
    auto& instances = bgl::Launcher::getSingleton().getInstances();
    for (auto& instance : instances) {
        if (instance->getName() == name) {
            instance->launch();
            break;
        }
    }
}
}

namespace bgl {
Launcher::Launcher() = default;

Launcher& Launcher::getSingleton()
{
    static Launcher singleton { };
    return singleton;
}

void Launcher::startLoop()
{
    // function table consists of vary actions
    std::unordered_map<std::string, std::function<void(std::string)>> actions;

    actions.insert_or_assign("about", &aboutAction);
    actions.insert_or_assign("download", &downloadAction);
    actions.insert_or_assign("exit", &shutDownAction);
    actions.insert_or_assign("help", &helpAction);
    actions.insert_or_assign("launch", &launchAction);
    actions.insert_or_assign("list", &listAction);
    actions.insert_or_assign("refresh", &refreshInstanceAction);

    printWelcome();

    while (true) {
        getSingleton().scanInstance();
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
        } else if (args.size() >= 3) {
            std::cout << "Too many arguments." << '\n';
            continue;
        } else if (arg.empty()) {
            continue;
        }
        try {
            actions[args.at(0)](args.at(1));
        } catch (const std::bad_function_call&) {
            std::cout << "Unknown command. Type help for command list." << '\n';
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
}
