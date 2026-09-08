//
// Created by littl on 2026/9/1.
//

#ifndef BGL_INSTANCE_H
#define BGL_INSTANCE_H

#include <filesystem>

namespace bgl {
class Instance {
public:
    explicit Instance(std::string name);

    // view
    [[nodiscard]] std::string getName() const;

    bool compare(const Instance& other) const;

    // actions
    bool downloadAndVerify();
    void launch();

private:
    std::string name_;
    std::string indexCode_;

    std::filesystem::path path_;
};
}

#endif // BGL_INSTANCE_H
