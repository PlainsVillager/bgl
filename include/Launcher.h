//
// Created by littl on 2026/9/1.
//

#ifndef BGL_LAUNCHER_H
#define BGL_LAUNCHER_H

#include <memory>
#include <string_view>
#include <vector>


#include "Instance.h"

namespace bgl {
class Launcher {
public:
    ~Launcher() = default;
    Launcher(const Launcher&) = delete;
    Launcher& operator=(const Launcher&) = delete;
    Launcher(Launcher&&) = delete;
    Launcher& operator=(Launcher&&) = delete;

    static Launcher& getSingleton();

    void startLoop();
    [[nodiscard]] std::vector<std::unique_ptr<Instance>>& getInstances();
    // [[nodiscard]] std::unique_ptr<Instance>& getInstance(const std::string& name);
    void scanInstance();

private:
    Launcher();
    std::vector<std::unique_ptr<Instance>> instances_ { };
};
}

namespace bgl::constants {
inline constexpr std::string_view LAUNCHER_VER_MAJOR_STR = "alpha";
inline constexpr std::string_view LAUNCHER_VER_MINOR_STR = "build Sep 6 2026 UTC 9: 08";
}

#endif // BGL_LAUNCHER_H
