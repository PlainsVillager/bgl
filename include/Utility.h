//
// Created by littl on 2026/9/1.
//

#ifndef BGL_UTILITY_H
#define BGL_UTILITY_H

#include <string>
#include <queue>

namespace bgl {
    void downloadFile(std::string url, std::string path);
    void multiThreadDownload(std::queue<std::pair<std::string, std::string>>& files);
}

#endif //BGL_UTILITY_H
