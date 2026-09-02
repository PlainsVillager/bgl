//
// Created by littl on 2026/9/1.
//
#include "Utility.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <format>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <cpr/cpr.h>

namespace {
    // for e.g.  a/b.txt
    bool downloadFile(const std::string& url, const std::string& path) {
        namespace fs = std::filesystem;
        std::string fileName = bgl::getFileName(url);
        fs::path fullPath{path + '/' + fileName};
        if (!fs::exists(path)) fs::create_directories(path);
        if (fs::exists(fullPath)) return true;

        std::cout << "Downloading " << url << "...";
        std::ofstream out(fullPath.c_str(), std::ios::binary);

        cpr::Response r = cpr::Download(out, cpr::Url{url});

        if (r.status_code != 200) {
            std::cerr << "Failed: " << r.status_code << " " << r.error.message << "\n";
            return false;
        }
        out.close();
        std::cout << "Complete" << std::endl;
        return true;
    }
}

namespace bgl {
    bool tryDownloadFile(const std::string& url, const std::string& path, std::size_t tryTimes) {
        std::size_t i = 0;
        while (i < tryTimes) {
            ++i;
            if (downloadFile(url, path)) return true;
        }
        return false;
    }

    // 尽可能少调用该方法
    void multiThreadDownload(std::queue<std::pair<std::string, std::string>>& files) {
        const std::size_t threadNum{std::thread::hardware_concurrency()};

        std::mutex mtx;
        std::condition_variable cv;
        std::atomic remainingTasks(files.size());
        bool stop = false;
        std::vector<std::thread> workers;
        workers.reserve(threadNum);

        for (size_t i = 0; i < threadNum; ++i) {
            workers.emplace_back([&] {
                while (true) {
                    std::pair<std::string, std::string> task;
                    {
                        std::unique_lock lock(mtx);
                        cv.wait(lock, [&]() { return stop || !files.empty(); });
                        if (stop && files.empty()) {
                            return;
                        }
                        task = files.front();
                        files.pop();
                    }
                    tryDownloadFile(task.first, task.second);
                    --remainingTasks;
                }
            });
        }
        while (remainingTasks > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        {
            std::lock_guard lock(mtx);
            stop = true;
        }
        cv.notify_all();

        for (auto& t : workers) {
            t.join();
        }
    }

    std::string getFileName(const std::string& urlOrPath) {
        const std::size_t slashPos = urlOrPath.find_last_of('/');
        return urlOrPath.substr(slashPos + 1);
    }
}
