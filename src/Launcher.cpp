//
// Created by littl on 2026/9/1.
//
#include "Launcher.h"
#include "Utility.h"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <format>
#include <vector>
#include <functional>
#include <unordered_map>
#include <nlohmann/json.hpp>

namespace {
    void printWelcome() {
        std::cout << "Bgl Minecraft Launcher Console Version " << bgl::constants::LAUNCHER_VER_MAJOR_STR << '.' <<
                bgl::constants::LAUNCHER_VER_MINOR_STR << std::endl;
    }

    void helpAction(std::string_view queryCmd) {
        if (queryCmd == "about") {
            std::cout << "About this program" << std::endl;
        } else if (queryCmd == "download") {
            std::cout << "Download a Minecraft instance from internet\n" <<
                    "command syntax: download <name: string>\n" <<
                    "param explanation: \"name\" for e.g. 26.2 or 20w06a or 26.3-snapshot-10 with no blank)" <<
                    std::endl;
        } else if (queryCmd == "exit") {
            std::cout << "Exit program." << std::endl;
        } else if (queryCmd == "help") {
            std::cout << "You can't query current command!" << std::endl;
        } else if (queryCmd == "list") {
            std::cout << "List local installed instances" << std::endl;
        } else if (queryCmd == "launch") {
            std::cout << "Launch a local Minecraft instance\n" <<
                    "command syntax: launch <name: string>\n" <<
                    "param explanation: \"name\" for e.g. 26.2 or 20w06a or 26.3-snapshot-10 with no blank)" <<
                    "param explanation: \"name\" for e.g. 26.2 or 20w06a or 26.3-snapshot-10 with no blank)" <<
                    std::endl;
        } else if (queryCmd.empty()) {
            std::cout << "about\ndownload\nexit\nhelp\nlist\nlaunch\nremove\nupdate" << std::endl;
        } else {
            std::cout << "Unknown command. Retry later" << std::endl;
        }
    }

    void aboutAction(std::string_view param) {
        std::cout << "A cli Minecraft Java Edition Launcher by PlainsVillager" << std::endl;
    }

    void shutDownAction(std::string_view param) {
        std::cout << "Shutting down." << std::endl;
        std::exit(0);
    }

    void downloadAction(const std::string& version) {
        if (version.empty()) {
            std::cout << "Please input a mc version name\n";
            return;
        }

        {
            auto& singleton = bgl::Launcher::getSingleton();
            singleton.scanInstances();
            auto& instances = singleton.getInstances();
            for (const auto& instance: instances) {
                if (instance.getName() == version) {
                    std::cout << "This version has been installed. So we will be checking file hash soon.\n";
                }
            }
        }

        bgl::Instance currentInst{version, false};
        int execCode = currentInst.download();
        if (execCode == 101) {
            std::cout << "Unknown version\n";
        }
    }

    void listAction(std::string_view param) {
        auto& singleton = bgl::Launcher::getSingleton(); // 使用引用
        auto& instances = singleton.getInstances();

        if (instances.empty()) {
            std::cout << "No instances are installed. Try install by 'download' command.\n";
            return;
        }
        std::cout << "Installed Minecraft instances are list below:\n";
        for (const auto& e: instances) {
            std::cout << e.getName() << '\n';
        }
    }

    void launchAction(std::string_view name) {
        if (name.empty()) {
            std::cout << "a mc version name must be given as the second param.\n";
            return;
        }
        auto& singleton = bgl::Launcher::getSingleton();
        auto& instances = singleton.getInstances();
        for (auto& e: instances) {
            if (e.getName() == name) {
                int code = e.launch();
                if (code != 0) {
                    std::cout << "Oops! Something went wrong when launching Minecraft.\n";
                }
                break;
            }
        }
    }
}

namespace bgl {
    Launcher& Launcher::getSingleton() {
        static Launcher singleton{};
        return singleton;
    }

    void Launcher::startLoop() {
        // function table consists of vary actions
        std::unordered_map<std::string, std::function<void(std::string)> > actions;

        actions.insert_or_assign("about", &aboutAction);
        actions.insert_or_assign("download", &downloadAction);
        actions.insert_or_assign("exit", &shutDownAction);
        actions.insert_or_assign("help", &helpAction);
        actions.insert_or_assign("launch", &launchAction);
        actions.insert_or_assign("list", &listAction);

        printWelcome();

        while (true) {
            getSingleton().scanInstances();
            std::cout << ">>";
            std::string cmd;
            std::getline(std::cin, cmd);

            std::vector<std::string> args{};
            std::istringstream iss{cmd};
            std::string arg;
            while (iss >> arg) {
                args.emplace_back(arg);
            }
            if (args.size() == 1) {
                args.emplace_back();
            } else if (args.size() >= 3) {
                std::cout << "Too many arguments." << std::endl;
                continue;
            } else if (arg.empty()) {
                continue;
            }
            try {
                actions[args.at(0)](args.at(1));
            } catch (const std::bad_function_call&) {
                std::cout << "Unknown command. Type help for command list." << std::endl;
            }
        }
    }

    void Launcher::scanInstances() {
        instances_.clear();
        namespace fs = std::filesystem;
        const fs::path versionsPath = ".minecraft/versions";
        if(!fs::exists(versionsPath)) return;
        for (const auto& entry: fs::directory_iterator(versionsPath)) {
            if (!entry.is_directory()) continue;
            auto jar = std::format("{}/client.jar", entry.path().generic_string());
            auto name = getFileName(entry.path().generic_string());
            auto json = std::format("{}/{}.json", entry.path().generic_string(), name);
            if (fs::exists(jar) && fs::exists(json)) {
                instances_.emplace_back(name, true);
            }
        }
    }

    std::vector<Instance>& Launcher::getInstances() {
        return instances_;
    }

    Launcher::Launcher() = default;
}
