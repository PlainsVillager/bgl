//
// Created by littl on 2026/9/1.
//
#include "Launcher.h"
#include "Utility.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <vector>
#include <functional>
#include <unordered_map>
#include <queue>
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
        bgl::tryDownloadFile("https://piston-meta.mojang.com/mc/game/version_manifest.json",
                             ".minecraft/versions");
        // 加载version_manifest.json
        std::ifstream ifs(".minecraft/versions/version_manifest.json");
        nlohmann::json manifest;
        ifs >> manifest;
        ifs.close();

        // 解析版本列表
        std::unordered_map<std::string, std::string> versions;
        for (const auto& elem: manifest["versions"]) {
            std::string id = elem["id"];
            std::string url = elem["url"];
            versions[id] = url;
        }
        if (!versions.contains(version)) {
            std::cout << "Unknown version." << std::endl;
            return;
        }

        // 下载版本json文件
        bgl::tryDownloadFile(versions[version], ".minecraft/versions/" + version);

        // 加载版本json文件
        ifs.open(".minecraft/versions/" + version + '/' + version + ".json");
        nlohmann::json verJson;
        ifs >> verJson;
        ifs.close();

        // 开始解析版本json文件
        //  下载客户端jar文件
        bgl::tryDownloadFile(verJson["downloads"]["client"]["url"],
                             ".minecraft/versions/" + version);
        //  下载资源索引文件
        bgl::tryDownloadFile(verJson["assetIndex"]["url"],
                             ".minecraft/assets/indexes");

        //  解析库文件列表
        std::unordered_map<std::string, std::string> libraries{};
        std::vector<std::string> natives{};
        for (const auto& elem: verJson["libraries"]) {
            std::string url = elem["downloads"]["artifact"]["url"];
            std::string var2 = elem["downloads"]["artifact"]["path"];
            std::string path = ".minecraft/libraries/" + var2;
            libraries[url] = path;
            natives.push_back(var2);
        }

        //  加载资源索引文件
        ifs.open(".minecraft/assets/indexes/32.json");
        nlohmann::json index;
        ifs >> index;
        ifs.close();
        //  下载assets
        //std::unordered_map<std::string, std::string> files{};
        std::queue<std::pair<std::string, std::string> > files{};
        for (auto [filePath, fileInfo]: index["objects"].items()) {
            std::string hashFull{fileInfo["hash"]};
            std::string hashFront{hashFull.substr(0, 2)};
            std::string url = "https://bmclapi2.bangbang93.com/assets/" + hashFront + "/" += hashFull;
            // bgl::downloadFile(url, ".minecraft/assets/objects/" + hashFront);
            files.emplace(url, ".minecraft/assets/objects/" + hashFront);
        }

        // 下载库文件
        for (const auto& [url, path]: libraries) {
            std::size_t slash = path.find_last_of('/');
            files.emplace(url, path.substr(0, slash));
        }
        //多线程下载
        bgl::multiThreadDownload(files);
    }

    void listAction(std::string_view param) {
        bgl::Launcher singleton = bgl::Launcher::getSingleton();
        const auto& instances = singleton.getInstances();

        if (instances.empty()) {
            std::cout<<"No instances are installed. Try install by 'download' command.";
            return;
        }
        std::cout<<"Installed Minecraft instances are list below:\n";
        for (const auto& e : instances) {
            std::cout<<e.getName()<<'\n';
        }
    }
}

namespace bgl {
    Launcher& Launcher::getSingleton() {
        static Launcher singleton{};
        return singleton;
    }

    Launcher::Launcher() = default;

    void Launcher::start() {
        // 函数表 不同命令对应不同操作逻辑
        std::unordered_map<std::string, std::function<void(std::string)> > actions;

        actions.insert_or_assign("about", &aboutAction);
        actions.insert_or_assign("download", &downloadAction);
        actions.insert_or_assign("exit", &shutDownAction);
        actions.insert_or_assign("help", &helpAction);
        actions.insert_or_assign("list", &listAction);

        printWelcome();

        while (true) {
            scanInstances();
            std::cout << ">>";
            std::string cmd;
            std::getline(std::cin, cmd);

            std::vector<std::string> args{};
            std::istringstream iss{cmd};
            std::string arg;
            while (iss >> arg) {
                args.emplace_back(arg);
            }
            if (args.size() >= 3) {
                std::cout << "Too many arguments." << std::endl;
            } else if (args.size() == 1) {
                args.emplace_back();
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
        for (const auto& entry : fs::directory_iterator(versionsPath)) {
            if (!fs::exists(entry) || !entry.is_directory()) break;
            auto jar = entry.path() / "client.jar";
            auto name = getFileName(entry.path().generic_string());
            auto json = entry.path() /  (name + ".json");
            if (fs::exists(jar) && fs::exists(json)) {
                instances_.emplace_back(name);
            }
        }
    }

    const std::vector<LocalInstance>& Launcher::getInstances() const {
        return instances_;
    }
}
