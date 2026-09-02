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
        static Launcher& getSingleton();
        void start();
        void scanInstances();
        [[nodiscard]] const std::vector<LocalInstance>& getInstances() const;
    private:
        Launcher();
        std::vector<LocalInstance> instances_{};
    };
}

namespace bgl::constants {
    // inline constexpr int LAUNCHER_VER_MAJOR = 0;
    // inline constexpr int LAUNCHER_VER_MINOR = 1;
    inline constexpr std::string_view LAUNCHER_VER_MAJOR_STR = "alpha";
    inline constexpr std::string_view LAUNCHER_VER_MINOR_STR = "alpha";

}

#endif //BGL_LAUNCHER_H
