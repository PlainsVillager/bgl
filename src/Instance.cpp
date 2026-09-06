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
        // load manifest
        std::ifstream ifs(".minecraft/versions/version_manifest.json");
        nlohmann::json manifest;
        ifs >> manifest;
        ifs.close();

        // parse list
        std::unordered_map<std::string, std::string> versions;
        for (const auto& elem: manifest["versions"]) {
            std::string id = elem["id"];
            std::string url = elem["url"];
            versions[id] = url;
        }
        if (!versions.contains(name_)) {
            return 101;
        }

        tryDownloadFile(versions[name_], ".minecraft/versions/" + name_);

        // load json
        ifs.open(".minecraft/versions/" + name_ + '/' + name_ + ".json");
        nlohmann::json verJson;
        ifs >> verJson;
        ifs.close();

        // parse json
        // parse index code
        std::string indexCode = verJson["assetIndex"]["id"];
        //  download client jar
        tryDownloadFile(verJson["downloads"]["client"]["url"],
                             ".minecraft/versions/" + name_);
        //  download index
        tryDownloadFile(verJson["assetIndex"]["url"],
                             ".minecraft/assets/indexes");

        //  parse libraries
        // std::unordered_map<std::string, std::string> libraries{};
        std::vector<std::string> librariesUrl{};
        std::vector<std::string> librariesHash{};
        std::vector<std::string> librariesPath{};
        librariesUrl.reserve(96);
        librariesHash.reserve(96);
        librariesPath.reserve(96);

        for (const auto& elem: verJson["libraries"]) {
            std::string url = elem["downloads"]["artifact"]["url"];
            std::string hash = elem["downloads"]["artifact"]["sha1"];
            std::filesystem::path artifactPath{elem["downloads"]["artifact"]["path"].get<std::string>()};
            std::string path = (std::filesystem::path{".minecraft/libraries"} / artifactPath.parent_path()).generic_string();
            librariesUrl.emplace_back(std::move(url));
            librariesHash.emplace_back(std::move(hash));
            librariesPath.emplace_back(std::move(path));
        }

        //  load index
        ifs.open(".minecraft/assets/indexes/" + indexCode + ".json");
        nlohmann::json index;
        ifs >> index;
        ifs.close();
        //  assets push queue
        std::queue<std::array<std::string, 3>> filesWithHash{};// order: url path hash
        for (auto [filePath, fileInfo]: index["objects"].items()) {
            std::string hashFull{fileInfo["hash"]};
            std::string hashFront{hashFull.substr(0, 2)};
            std::string url = "https://bmclapi2.bangbang93.com/assets/" + hashFront + "/" += hashFull;
            filesWithHash.emplace(std::array{std::move(url), ".minecraft/assets/objects/" + hashFront, std::move(hashFull)});
        }

        for (std::size_t i = 0; i < librariesUrl.size(); ++i) {
            filesWithHash.emplace(std::array{std::move(librariesUrl[i]), std::move(librariesPath[i]) ,std::move(librariesHash[i])});
        }
        //多线程下载
        multiThreadDownload(filesWithHash);
        local_ = true;
        return 0;
    }

    /// @return 0 success
    /// @return 1 fail
    int Instance::launch() {
        std::string args{};
        args.append("@echo off\n");
        namespace fs = std::filesystem;
        fs::path nativePath = fs::absolute(std::format(".minecraft/versions/{}/natives", name_));
        args.append(std::format("java -Djava.library.path={} ", nativePath.generic_string()));
        args.append("-cp ");

        //load json file
        std::ifstream ifs;
        ifs.open(std::format(".minecraft/versions/{0}/{0}.json", name_));
        nlohmann::json json;
        ifs >> json;
        ifs.close();


        for (const auto& elem: json["libraries"]) {
            {
                std::string relativePath = elem["downloads"]["artifact"]["path"];
                args.append(fs::absolute(std::format(".minecraft/libraries/{}", relativePath)).generic_string());
                args.append(";");
            }
        }
        args.append(fs::absolute(std::format(".minecraft/versions/{}/client.jar", name_)).generic_string());
        
        args.append(" ");

        args.append("net.minecraft.client.main.Main ");
        args.append("--username \"steve\" ");
        args.append(std::format("--version \"{}\" ", name_));
        args.append(std::format("--gameDir \"{}\" ", fs::absolute(std::format(".minecraft/versions/{}", name_)).generic_string()));
        args.append(std::format("--assetsDir \"{}\" ", fs::absolute(".minecraft/assets").generic_string()));
        args.append(std::format("--assetIndex {} ", indexCode_));
        args.append("--uuid 380df991f603344ca090369bad2a924a --accessToken c09158f8ac46412d8a9f142833993627 ");

        auto launchBat{"args.bat"};
        std::ofstream ofs(launchBat);
        ofs<<args;
        ofs.close();

        system("args.bat");

        return 0;
    }
}
