//
// Created by littl on 2026/9/1.
//

#ifndef BGL_LOCALINSTANCE_H
#define BGL_LOCALINSTANCE_H

#include <string_view>
#include <filesystem>

namespace bgl {
    class LocalInstance {
    public:
        explicit LocalInstance(const std::string& name);
        [[nodiscard]] std::string_view getName() const;
        // verify()
        // launch()
    private:
        std::string_view name_;
        std::filesystem::path path_;
    };
}

#endif //BGL_LOCALINSTANCE_H
