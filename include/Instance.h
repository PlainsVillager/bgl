//
// Created by littl on 2026/9/1.
//

#ifndef BGL_INSTANCE_H
#define BGL_INSTANCE_H

#include <filesystem>

namespace bgl {
    class Instance {
    public:
        explicit Instance(std::string name, bool local);

        //view
        [[nodiscard]] std::string getName() const;
        [[nodiscard]] bool isLocal() const;
        void setLocal(bool val);

        //actions
        int download(); // verify
        int launch();

    private:
        std::string name_;
        std::filesystem::path path_;
        int indexCode_;
        bool local_;
    };
}

#endif //BGL_INSTANCE_H
