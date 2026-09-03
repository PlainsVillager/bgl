//
// Created by littl on 2026/9/2.
//

#include "LocalInstance.h"
#include <fstream>
#include <filesystem>
#include <format>
#include <vector>
#include <nlohmann/json.hpp>

namespace bgl {
    LocalInstance::LocalInstance(std::string name) :  name_(name), path_(".minecraft/versions/" + name) {
        std::ifstream ifs;
        ifs.open(".minecraft/versions/" + std::string{name_} + '/' + std::string{name_} + ".json");
        nlohmann::json verJson;
        ifs >> verJson;
        ifs.close();
        indexCode_ = std::stoi(std::string{verJson["assetIndex"]["id"]});
    }

    std::string LocalInstance::getName() const {
        return name_;
    }

    /// @return 0 success
    /// @return 1 fail
    int LocalInstance::launch() const {
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
        execCmd.append(std::format("--gamedir \"{}\" ", fs::absolute(std::format(".minecraft/versions/{}", name_)).generic_string()));
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
