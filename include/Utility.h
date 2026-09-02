//
// Created by littl on 2026/9/1.
//

#ifndef BGL_UTILITY_H
#define BGL_UTILITY_H

#include <string>
#include <queue>

namespace bgl {
    [[deprecated]] bool downloadFile(std::string url, std::string path);
    bool tryDownloadFile(std::string url, std::string path, std::size_t tryTimes = 3);
    void multiThreadDownload(std::queue<std::pair<std::string, std::string>>& files);
    std::string getFileName(std::string urlOrPath);
}

#endif //BGL_UTILITY_H
