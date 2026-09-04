//
// Created by littl on 2026/9/2.
//

#include "Instance.h"
#include "Utility.h"
#include <fstream>
#include <filesystem>
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

namespace bgl {
    Instance::Instance(std::string name, bool local) :  name_(name), path_(".minecraft/versions/" + name), local_(local) {
        std::ifstream ifs;
        ifs.open(".minecraft/versions/" + std::string{name_} + '/' + std::string{name_} + ".json");
        if (ifs.is_open()) {
            nlohmann::json verJson;
            ifs >> verJson;
            ifs.close();
            indexCode_ = std::stoi(std::string{verJson["assetIndex"]["id"]});
        }else {
            indexCode_ = 0;
        }
    }

    std::string Instance::getName() const {
        return name_;
    }

    bool Instance::isLocal() const {
        return local_;
    }

    void Instance::setLocal(bool val) {
        local_ = val;
    }

    //101 unknown version
    //0 success
    int Instance::download() {
        tryDownloadFile("https://piston-meta.mojang.com/mc/game/version_manifest.json",
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
        if (!versions.contains(name_)) {
            return 101;
        }

        // 下载版本json文件
        tryDownloadFile(versions[name_], ".minecraft/versions/" + name_);

        // 加载版本json文件
        ifs.open(".minecraft/versions/" + name_ + '/' + name_ + ".json");
        nlohmann::json verJson;
        ifs >> verJson;
        ifs.close();

        // 开始解析版本json文件
        // 解析index代码
        std::string indexCode = verJson["assetIndex"]["id"];
        //  下载客户端jar文件
        tryDownloadFile(verJson["downloads"]["client"]["url"],
                             ".minecraft/versions/" + name_);
        //  下载资源索引文件
        tryDownloadFile(verJson["assetIndex"]["url"],
                             ".minecraft/assets/indexes");

        //  解析库文件列表
        std::unordered_map<std::string, std::string> libraries{};

        for (const auto& elem: verJson["libraries"]) {
            std::string url = elem["downloads"]["artifact"]["url"];
            std::string var2 = elem["downloads"]["artifact"]["path"];
            std::string path = ".minecraft/libraries/" + var2;
            libraries[url] = path;
        }

        //  加载资源索引文件
        ifs.open(".minecraft/assets/indexes/" + indexCode + ".json");
        nlohmann::json index;
        ifs >> index;
        ifs.close();
        //  assets push queue
        std::queue<std::pair<std::string, std::string> > files{};
        for (auto [filePath, fileInfo]: index["objects"].items()) {
            std::string hashFull{fileInfo["hash"]};
            std::string hashFront{hashFull.substr(0, 2)};
            std::string url = "https://bmclapi2.bangbang93.com/assets/" + hashFront + "/" += hashFull;
            files.emplace(url, ".minecraft/assets/objects/" + hashFront);
        }

        // libraries push
        for (const auto& [url, path]: libraries) {
            std::size_t slash = path.find_last_of('/');
            files.emplace(url, path.substr(0, slash));
        }
        //多线程下载
        multiThreadDownload(files);
        local_ = true;
        return 0;
    }

    /// @return 0 success
    /// @return 1 fail
    int Instance::launch() {
        std::string execCmd{};
        namespace fs = std::filesystem;
        fs::path nativePath = fs::absolute(std::format(".minecraft/versions/{}/natives", name_));
        execCmd.append(std::format("java -Djava.library.path={} ", nativePath.generic_string()));
        execCmd.append("-cp ");

        //load json file
        std::ifstream ifs;
        ifs.open(std::format(".minecraft/versions/{0}/{0}.json", name_));
        nlohmann::json json;
        ifs >> json;
        ifs.close();


        for (const auto& elem: json["libraries"]) {
            {
                std::string relativePath = elem["downloads"]["artifact"]["path"];
                execCmd.append(fs::absolute(std::format(".minecraft/libraries/{}", relativePath)).generic_string());
                execCmd.append(";");
            }
        }
        execCmd.append(fs::absolute(std::format(".minecraft/versions/{}/client.jar", name_)).generic_string());
        execCmd.append(" net.minecraft.client.main.Main ");
        execCmd.append("--username \"steve\" ");
        execCmd.append(std::format("--version \"{}\" ", name_));
        execCmd.append(std::format("--gameDir \"{}\" ", fs::absolute(std::format(".minecraft/versions/{}", name_)).generic_string()));
        execCmd.append(std::format("--assetsDir \"{}\" ", fs::absolute(".minecraft/assets").generic_string()));
        execCmd.append(std::format("--assetIndex {} ", indexCode_));
        execCmd.append("--uuid 380df991f603344ca090369bad2a924a --accessToken c09158f8ac46412d8a9f142833993627 ");

        auto launchBat{"launch.bat"};
        std::ofstream ofs(launchBat);
        ofs<<execCmd;
        ofs.close();
        system(launchBat);

        return 0;
    }
}
