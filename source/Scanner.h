#pragma once

#include "Config.h"
#include <string>
#include <unordered_set>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <algorithm>
#include <system_error>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <set>

class Scanner {
public:
    Scanner(const Config& p_Config);

    void Run();

private:
    Config m_config;
    std::unordered_set<std::string> m_iocHashes;
    std::vector<std::filesystem::path> m_filesToScan;
    std::vector<std::filesystem::path> m_foldersFound;

    void LoadIocHashes();
    void CollectFiles(const std::filesystem::path &p_directory);
    std::string ComputeSha256(const std::filesystem::path& p_filePath);
    bool CheckExtension(const std::filesystem::path& p_filePath);

    bool IsPrunedPath(const std::filesystem::path& p_path);

    void ScanFiles();
    void WorkerThread(size_t total);
    void ScanFileTask(const std::filesystem::path& p_file, size_t p_total);

    // PROGRESS BAR
    std::mutex m_outputMutex;
    std::mutex m_errorMutex;
    std::mutex m_progressMutex;
    size_t m_progressCount = 0;

    void PrintProgress(size_t p_current, size_t p_total);

    /// THREAD
    std::mutex m_queueMutex;
    std::condition_variable m_cv;
    std::queue<std::filesystem::path> m_taskQueue;
    bool m_stopThreads = false;
};