//
// Created by littl on 2026/9/1.
//
#include "Utility.h"
#include "sha1.hpp"
#include <algorithm>
#include <condition_variable>
#include <cpr/cpr.h>
#include <cstddef>
#include <exception>
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
bool downloadFile(const std::string& url, const std::string& path, const std::string& sha1) // NOLINT
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
} // namespace

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
    /* std::size_t threadNum { std::thread::hardware_concurrency() };
    if (threadNum == 0)
        threadNum = 1;

    std::mutex mtx1;
    std::condition_variable cv;
    std::atomic remainingTasks(files.size());
    std::atomic_bool stop = false;
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
                    stop = true;
                    cv.notify_all();
                }
            }
        });
    }
    while (remainingTasks > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    stop = true;

    cv.notify_all();

    for (auto& t : workers) {
        t.join();
    } */

    // clang-format off
    // ---------------------------------------------------------------------------
    // v2 (previous attempt, kept for reference)
    // Problems of this version:
    //   1. 'stop' is only set on failure, so a fully successful run never
    //      terminates: the workers go back to cv.wait and the main thread polls
    //      forever.
    //   2. 'files.empty() && stop' means "only quit once the queue is drained",
    //      so a failure does NOT stop the remaining tasks.
    //   3. stop.store(true) does not wake up threads blocked in cv.wait; they
    //      only wake up after the main thread's notify_all(), i.e. up to 1s later.
    //   4. The main thread polls with sleep_for instead of waiting on the cv.
    //   5. std::array<std::string, 3> file { files.front() } copies the task
    //      (3 string allocations) before it is popped.
    //   6. The redundant stop.store(true) inside the empty-check mixes the
    //      "finished" and "aborted" meanings into one flag.
    //   7. No exception safety: any throw inside a worker calls std::terminate.
    //
    // std::size_t threadsNum { std::max(1u, std::thread::hardware_concurrency()) };
    // std::vector<std::thread> workers;
    // workers.reserve(threadsNum);
    // std::mutex mtx;
    // std::condition_variable cv;
    // std::atomic_bool stop { false };
    //
    // for (std::size_t i { }; i < threadsNum; ++i) {
    //     workers.emplace_back([&] {
    //         while (true) {
    //             std::unique_lock lck { mtx };
    //             cv.wait(lck, [&] { return !files.empty() || stop; });
    //             if (files.empty() && stop.load()) {
    //                 stop.store(true);
    //                 break;
    //             }
    //             std::array<std::string, 3> file { files.front() };
    //             files.pop();
    //             lck.unlock();
    //
    //             bool downloadResult = tryDownloadFile(std::move(file[0]), std::move(file[1]), 3, std::move(file[2]));
    //             if (!downloadResult) {
    //                 stop.store(true);
    //             }
    //         }
    //     });
    // }
    //
    // while (true) {
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    //     if (stop.load())
    //         break;
    // }
    // cv.notify_all();
    //
    // std::ranges::for_each(workers, [](auto& worker) { worker.join(); });
    // ---------------------------------------------------------------------------

    // ------------------------------- v3 ----------------------------------------
    // Two distinct states instead of one overloaded flag:
    //   finished -> every queued task completed successfully
    //   abort    -> one task failed (or a worker threw), stop as soon as possible
    // Both are plain bools protected by 'mtx' so that every state change can be
    // paired with cv.notify_all() while holding the lock.
    const std::size_t threadsNum {
        std::clamp(static_cast<std::size_t>(std::thread::hardware_concurrency()), std::size_t { 1 }, std::size_t { 8 })
    };

    std::mutex mtx;
    std::condition_variable cv;
    std::size_t remaining { files.size() }; // tasks neither finished nor failed yet
    bool finished { remaining == 0 };       // nothing to do at all
    bool abort { false };

    std::vector<std::thread> workers;
    workers.reserve(threadsNum);

    for (std::size_t i { }; i < threadsNum; ++i) {
        workers.emplace_back([&] {
            // A worker must never let an exception escape: it would call
            // std::terminate() and leave the main thread waiting forever.
            try {
                while (true) {
                    std::array<std::string, 3> file; // order: url path sha1
                    {
                        std::unique_lock lck { mtx };
                        cv.wait(lck, [&] { return finished || abort || !files.empty(); });
                        if (finished || abort)
                            return;

                        file = std::move(files.front()); // move instead of copy
                        files.pop();
                    }

                    if (!tryDownloadFile(std::move(file[0]), std::move(file[1]), 3, std::move(file[2]))) {
                        std::unique_lock lck { mtx };
                        abort = true;
                        cv.notify_all();
                        return;
                    }

                    std::unique_lock lck { mtx };
                    if (--remaining == 0) {
                        finished = true;
                        cv.notify_all();
                        return;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Download worker aborted: " << e.what() << '\n';
                std::unique_lock lck { mtx };
                abort = true;
                cv.notify_all();
            }
        });
    }

    // Block until the whole batch finishes or a failure happens; no polling.
    {
        std::unique_lock lck { mtx };
        cv.wait(lck, [&] { return finished || abort; });
    }

    std::ranges::for_each(workers, [](auto& worker) { worker.join(); });

    if (abort)
        std::cerr << "multiThreadDownload: aborted, the remaining files were not downloaded\n";
    // clang-format on
}

std::string getFileName(const std::string& urlOrPath)
{
    const std::size_t slashPos = urlOrPath.find_last_of('/');
    return urlOrPath.substr(slashPos + 1);
}

std::string generateUUID()
{
    return "TODO";
}

} // namespace bgl
