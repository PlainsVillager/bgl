//
// Created by littl on 2026/9/1.
//

#ifndef BGL_LAUNCHER_H
#define BGL_LAUNCHER_H


namespace bgl {
    class Launcher {
    public:
        static Launcher& getSingleton();
        void start();
    private:
        Launcher();
    };
}

namespace bgl::constants {
    inline constexpr int LAUNCHER_VER_MAJOR = 0;
    inline constexpr int LAUNCHER_VER_MINOR = 1;
    // inline constexpr std::string_view LAUNCHER_VER_FULL_STR = std::to_string(LAUNCHER_VER_MAJOR) + '.' + std::to_string(LAUNCHER_VER_MINOR);
    inline constexpr int MAX_DOWNLOAD_THREADS = 8;
}

#endif //BGL_LAUNCHER_H
