//
// Created by littl on 2026/9/1.
//

#ifndef BGL_LAUNCHER_H
#define BGL_LAUNCHER_H

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
        void scanInstances();
        [[nodiscard]] std::vector<Instance>& getInstances() ;
    private:
        Launcher();
        std::vector<Instance> instances_{};
    };
}

namespace bgl::constants {
    inline constexpr std::string_view LAUNCHER_VER_MAJOR_STR = "alpha";
    inline constexpr std::string_view LAUNCHER_VER_MINOR_STR = "build Sep 4 2026 UTC 7:32";
}

#endif //BGL_LAUNCHER_H
