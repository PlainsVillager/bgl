//
// Created by littl on 2026/9/1.
//
#include "Utility.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <format>
#include <vector>
#include <thread>
#include <cpr/cpr.h>

namespace {
    std::string getFileName(std::string urlOrPath) {
        std::size_t slashPos = urlOrPath.find_last_of('/');
        return urlOrPath.substr(slashPos + 1);
    }
}

namespace bgl {
    namespace fs = std::filesystem;

    // for e.g.  a/b.txt
    void downloadFile(std::string url, std::string path) {
        //todo
        std::string fileName = getFileName(url);
        fs::path fullPath{path + '/' + fileName};
        if (!fs::exists(path)) fs::create_directories(path);
        if (fs::exists(fullPath)) return;

        std::cout << "Downloading " << url << "...";
        std::ofstream out(fullPath.c_str(), std::ios::binary); // 必须 binary，否则 Windows 下会篡改字节

        cpr::Response r = cpr::Download(out, cpr::Url{url});

        if (r.status_code != 200) {
            // status_code == 0 表示根本没连上，看 r.error
            std::cerr << "Failed: " << r.status_code << " " << r.error.message << "\n";
            return;
        }
        out.close();
        std::cout << "Complete" << std::endl;
    }

    void multiThreadDownload(std::queue<std::pair<std::string, std::string>>& files) {
        std::size_t threadNum{std::thread::hardware_concurrency()}; if (threadNum == 0) threadNum = 1;

        //AIGC
        std::mutex queueMutex;
        std::condition_variable cv;
        std::atomic<size_t> remainingTasks(files.size());
        bool stop = false;
        std::vector<std::thread> workers;
        workers.reserve(threadNum);
        for (size_t i = 0; i < threadNum; ++i) {
            workers.emplace_back([&]() {
                while (true) {
                    std::pair<std::string, std::string> task;
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        // 等待直到有任务可做或收到停止信号
                        cv.wait(lock, [&]() { return stop || !files.empty(); });
                        if (stop && files.empty()) {
                            return; // 退出线程
                        }
                        task = files.front();
                        files.pop();
                    }
                    downloadFile(task.first, task.second);
                    --remainingTasks;
                }
            });
        }
        // 等待所有任务完成（简单的轮询，也可用条件变量优化）
        while (remainingTasks > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // 设置停止标志并唤醒所有线程
        {
            std::lock_guard lock(queueMutex);
            stop = true;
        }
        cv.notify_all();

        // 等待所有线程结束
        for (auto& t : workers) {
            t.join();
        }
    }
}
