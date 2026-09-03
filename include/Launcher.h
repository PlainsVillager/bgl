//
// Created by littl on 2026/9/1.
//

#ifndef BGL_LAUNCHER_H
#define BGL_LAUNCHER_H

#include <string_view>
#include <vector>

#include "LocalInstance.h"

namespace bgl {
    class Launcher {
    public:
        Launcher(const Launcher&) = delete;
        Launcher& operator=(const Launcher&) = delete;
        static Launcher& getSingleton();
        void start();
        void scanInstances();
        [[nodiscard]] std::vector<LocalInstance>& getInstances() ;
    private:
        Launcher();
        std::vector<LocalInstance> instances_{};
    };
}

namespace bgl::constants {
    inline constexpr std::string_view LAUNCHER_VER_MAJOR_STR = "alpha";
    inline constexpr std::string_view LAUNCHER_VER_MINOR_STR = "build Sep 3 2026 UTC 10:18";
}

#endif //BGL_LAUNCHER_H
