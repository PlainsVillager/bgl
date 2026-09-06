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
    namespace fs = std::filesystem;
    bool compareSHA1(const fs::path& fullPath, const std::string& sha1) {
        //do sth
        // if (sha1.empty()) return true;
        return true;
    }

    // for e.g.  a/b.txt
    bool downloadFile(const std::string& url, const std::string& path, const std::string& sha1) {

        std::string fileName = bgl::getFileName(url);
        fs::path fullPath{path + '/' + fileName};
        if (!fs::exists(path)) fs::create_directories(path);

        if (fs::exists(fullPath)) {
            //todo: hash verify
            if (compareSHA1(fullPath, sha1)) return true;
        }

        std::cout << "Downloading " << url << "...";
        std::ofstream out(fullPath.c_str(), std::ios::binary);

        cpr::Response r = cpr::Download(out, cpr::Url{url});

        if (r.status_code != 200) {
            std::cerr << "Failed: " << r.status_code << " " << r.error.message << '\n';
            return false;
        }
        out.close();
        std::cout << "Complete\n";
        return true;
    }
}

namespace bgl {
    bool tryDownloadFile(const std::string& url, const std::string& path, const std::size_t tryTimes, const std::string& sha1) {
        std::size_t i = 0;
        while (i < tryTimes) {
            ++i;
            if (downloadFile(url, path, sha1)) return true;
        }
        std::cout<<url<<"download failed\n";
        return false;
    }

    void multiThreadDownload(std::queue<std::array<std::string, 3>>& files) {
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
                    //todo: use move semantics to avoid copying
                    std::array<std::string, 3> task; // order: url path sha1
                    {
                        std::unique_lock lock(mtx);
                        cv.wait(lock, [&]() { return stop || !files.empty(); });
                        if (stop && files.empty()) {
                            return;
                        }
                        task = std::move(files.front());
                        files.pop();
                    }
                    tryDownloadFile(task[0], task[1], 3, task[2]);
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
