//
// Created by littl on 2026/9/1.
//

#ifndef BGL_UTILITY_H
#define BGL_UTILITY_H

#include <array>
#include <queue>
#include <string>

namespace bgl {
bool tryDownloadFile(std::string&& url, std::string&& path, std::size_t tryTimes = 3, std::string&& sha1 = { });
void multiThreadDownload(std::queue<std::array<std::string, 3>>& files);
std::string getFileName(const std::string& urlOrPath);
}

#endif // BGL_UTILITY_H
