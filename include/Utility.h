//
// Created by littl on 2026/9/1.
//

#ifndef BGL_UTILITY_H
#define BGL_UTILITY_H

#include <string>
#include <queue>
#include <array>

namespace bgl {
    bool tryDownloadFile(const std::string& url, const std::string& path, std::size_t tryTimes = 3, const std::string& sha1 = {});
    void multiThreadDownload(std::queue<std::array<std::string, 3>>& files);
    std::string getFileName(const std::string& urlOrPath);
}

#endif //BGL_UTILITY_H
