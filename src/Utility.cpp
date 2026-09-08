//
// Created by littl on 2026/9/1.
//
#include "Utility.h"
#include "sha1.hpp"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cpr/cpr.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace {
namespace fs = std::filesystem;

// todo: method not implemented yet
bool compareSHA1(const fs::path& fullPath, const std::string& expectedSHA1)
{
    // clang-format off
    if (expectedSHA1.empty()) return true;
    auto calculatedSHA1 =  SHA1::from_file(fs::absolute(fullPath).generic_string());
    if(calculatedSHA1 == "FAILED TO LOAD FILE") return false;
    if(calculatedSHA1 == expectedSHA1) return true;
    else return false;
    // clang-format on
}

// path: whole path of the file e.g Directory/FileName.txt
// sha1: SHA-1 value of the file. This will be checked when the file already exists
// todo: use a high concurrency friendly method
bool downloadFile(const std::string& url, const std::string& path, const std::string& sha1)
{

    std::string fileName = bgl::getFileName(url);
    fs::path fullPath { path + '/' + fileName };
    if (!fs::exists(path))
        fs::create_directories(path);

    if (fs::exists(fullPath)) {
        // todo: hash verify
        if (compareSHA1(fullPath, sha1)) {
            return true;
        } else {
            fs::remove(fullPath);
        }
    }

    std::ofstream out(fullPath.c_str(), std::ios::binary);
    if (!out) {
        return false;
    }

    cpr::Response r = cpr::Download(out, cpr::Url { url });

    if (r.status_code != 200) {
        std::cerr << "Failed: " << r.status_code << " " << r.error.message << " will retry\n";
        return false;
    }
    out.close();
    return true;
}
}

namespace bgl {
// todo: use a high concurrency friendly print function instead of stdout
bool tryDownloadFile(std::string&& url, std::string&& path, const std::size_t tryTimes, std::string&& sha1)
{

    for (std::size_t i = 0; i < tryTimes; ++i) {
        // clang-format off
        if(downloadFile(url, path, sha1)) return true;
        // clang-format on
    }
    std::cout << url << "Download failed. won't retry\n";
    return false;
}

// todo(***): if one worker failed, stop all workers and publish this message
// todo(**): performance optimization
void multiThreadDownload(std::queue<std::array<std::string, 3>>& files)
{
    static std::size_t threadNum { std::thread::hardware_concurrency() };
    if (threadNum == 0)
        threadNum = 1;

    std::mutex mtx1, mtx2;
    std::condition_variable cv;
    std::atomic remainingTasks(files.size());
    bool stop = false;
    std::vector<std::thread> workers;
    workers.reserve(threadNum);

    for (size_t i = 0; i < threadNum; ++i) {
        workers.emplace_back([&] {
            while (true) {
                std::array<std::string, 3> task; // order: url path sha1
                {
                    std::unique_lock lock(mtx1);
                    cv.wait(lock, [&]() { return stop || !files.empty(); });
                    if (stop && files.empty()) {
                        return;
                    }
                    task = std::move(files.front());
                    files.pop();
                }
                bool isSuccess = tryDownloadFile(std::move(task[0]), std::move(task[1]), 3, std::move(task[2]));
                --remainingTasks;
                if (!isSuccess) {
                    std::lock_guard<std::mutex> lock(mtx2);
                    stop = true;
                }
            }
        });
    }
    while (remainingTasks > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    {
        std::lock_guard lock(mtx1);
        stop = true;
    }
    cv.notify_all();

    for (auto& t : workers) {
        t.join();
    }
}

std::string getFileName(const std::string& urlOrPath)
{
    const std::size_t slashPos = urlOrPath.find_last_of('/');
    return urlOrPath.substr(slashPos + 1);
}
}
