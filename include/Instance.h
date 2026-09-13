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

    // actions
    bool downloadAndVerify();
    void launch(const std::string& name, const std::string& uuid);

private:
    std::string name_;
    std::string indexCode_;

    std::filesystem::path path_;
};
} // namespace bgl

#endif // BGL_INSTANCE_H
