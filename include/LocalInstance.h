//
// Created by littl on 2026/9/1.
//

#ifndef BGL_LOCALINSTANCE_H
#define BGL_LOCALINSTANCE_H

#include <filesystem>

namespace bgl {
    class LocalInstance {
    public:
        explicit LocalInstance(std::string name);
        [[nodiscard]] std::string getName() const;
        // verify()
        int launch() const;
    private:
        std::string name_;
        std::filesystem::path path_;
        int indexCode_;
    };
}

#endif //BGL_LOCALINSTANCE_H
