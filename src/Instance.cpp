//
// Created by littl on 2026/9/2.
//

#include "Instance.h"
#include "Utility.h"
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>

namespace {
std::unique_ptr<nlohmann::json> loadJson(std::ifstream& ifs, const std::string& path)
{
    if (ifs.is_open())
        ifs.close();
    ifs.open(path);
    if (!ifs.is_open()) {
        std::cerr << std::format("Error when trying to load {}\n", path);
        return nullptr;
    }
    auto loaded { std::make_unique<nlohmann::json>() };
    ifs >> *loaded;
    ifs.close();
    return loaded;
}
}

namespace bgl {
Instance::Instance(std::string name)
    : name_(name)
    , path_(".minecraft/versions/" + name)
    , indexCode_("")
{
}

std::string Instance::getName() const
{
    return name_;
}

/// @brief download a instance according to `name_`
bool Instance::downloadAndVerify()
{
    tryDownloadFile("https://piston-meta.mojang.com/mc/game/version_manifest_v2.json",
        ".minecraft/versions");
    // load manifest
    std::ifstream ifs;
    auto manifest = loadJson(ifs, ".minecraft/versions/version_manifest_v2.json");
    if (!manifest)
        return false;

    // parse list
    // std::unordered_map<std::string, std::string> versions;
    std::unordered_map<std::string, std::pair<std::string, std::string>> versions; // id url sha1
    for (const auto& elem : (*manifest)["versions"]) {
        std::string id = elem["id"];
        std::string url = elem["url"];
        std::string sha1 = elem["sha1"];
        versions[id] = std::make_pair(url, sha1);
    }
    if (!versions.contains(name_)) {
        return false;
    }

    tryDownloadFile(std::move(versions[name_].first),
        ".minecraft/versions/" + name_, 3,
        std::move(versions[name_].second));

    // load json
    auto verJson = loadJson(ifs, ".minecraft/versions/" + name_ + '/' + name_ + ".json");
    // not loaded
    if (!verJson)
        return false;

    // parse json
    // parse index code
    indexCode_ = (*verJson)["assetIndex"]["id"];

    //  download client jar
    tryDownloadFile((*verJson)["downloads"]["client"]["url"],
        ".minecraft/versions/" + name_,
        3, (*verJson)["downloads"]["client"]["sha1"]);
    //  download index
    tryDownloadFile((*verJson)["assetIndex"]["url"],
        ".minecraft/assets/indexes");

    //  parse libraries
    std::vector<std::string> librariesUrl;
    std::vector<std::string> librariesHash;
    std::vector<std::string> librariesPath;

    for (const auto& elem : (*verJson)["libraries"]) {
        std::string url = elem["downloads"]["artifact"]["url"];
        std::string hash = elem["downloads"]["artifact"]["sha1"];
        std::filesystem::path artifactPath { elem["downloads"]["artifact"]["path"].get<std::string>() };
        std::string path = (std::filesystem::path { ".minecraft/libraries" } / artifactPath.parent_path()).generic_string();
        librariesUrl.emplace_back(std::move(url));
        librariesHash.emplace_back(std::move(hash));
        librariesPath.emplace_back(std::move(path));
    }

    //  load index
    auto index = loadJson(ifs, ".minecraft/assets/indexes/" + indexCode_ + ".json");
    if (!index)
        return false;
    //  assets push queue
    std::queue<std::array<std::string, 3>> filesWithHash { }; // order: url path hash
    for (auto [filePath, fileInfo] : (*index)["objects"].items()) {
        std::string hashFull { fileInfo["hash"] };
        std::string hashFront { hashFull.substr(0, 2) };
        std::string url = "https://bmclapi2.bangbang93.com/assets/" + hashFront + "/" += hashFull;
        filesWithHash.emplace(std::array { std::move(url), ".minecraft/assets/objects/" + hashFront, std::move(hashFull) });
    }

    for (std::size_t i = 0; i < librariesUrl.size(); ++i) {
        filesWithHash.emplace(std::array { std::move(librariesUrl[i]), std::move(librariesPath[i]), std::move(librariesHash[i]) });
    }
    multiThreadDownload(filesWithHash);

    return true;
}

// todo replace system(const char* cmd)
void Instance::launch()
{
    std::cout << "Downloading and verifying specified version\n";

    if (!downloadAndVerify()) {
        std::cerr << "Fatal: Failed to download and verify\n";
    }

    std::string args { };
    args.append("@echo off\n");
    namespace fs = std::filesystem;
    fs::path nativePath = fs::absolute(std::format(".minecraft/versions/{}/natives", name_));
    args.append(std::format("java -Djava.library.path={} ", nativePath.generic_string()));
    args.append("-cp ");

    // load json file
    std::ifstream ifs;
    ifs.open(std::format(".minecraft/versions/{0}/{0}.json", name_));
    nlohmann::json json;
    ifs >> json;
    ifs.close();

    for (const auto& elem : json["libraries"]) {
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

    auto launchBat { "args.bat" };
    std::ofstream ofs(launchBat);
    ofs << args;
    ofs.close();

    system("args.bat");
}
}
