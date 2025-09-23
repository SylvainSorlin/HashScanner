#include "Scanner.h"
#include <openssl/sha.h>
#include <cstring>

Scanner::Scanner(const Config& p_Config)
    : m_config(p_Config)
{

}

void Scanner::Run() {
    // Clear old output files
    std::ofstream(m_config.Output_file(), std::ios::trunc).close();
    std::ofstream(m_config.Error_file(), std::ios::trunc).close();

    if (!std::filesystem::exists(m_config.Ioc_file())) {
        m_config.LogError("Error: cannot find IOC file: " + m_config.Ioc_file().string());
        exit(1);
    }

    std::cout<< "Info: Loading Hashes" <<std::endl;
    LoadIocHashes();
    std::cout<< "Info: Collecting Files List..." <<std::endl;
    CollectFiles(m_config.Scan_dir());
    ScanFiles();
}

void Scanner::LoadIocHashes() {
    std::ifstream l_file(m_config.Ioc_file());
    if (!l_file.is_open()) {
        m_config.LogError("Cannot open IOC file: " + m_config.Ioc_file().string());
        exit(1);
    }

    std::string l_line;
    while (std::getline(l_file, l_line)) {
        if (!l_line.empty()) {
            if (l_line.back() == '\r')
            {
                l_line.pop_back();
            }
            m_iocHashes.insert(l_line);
        }
    }
}

void Scanner::CollectFiles(const std::filesystem::path &p_directory) {
    std::error_code l_ec;
    std::filesystem::path l_dirPath(p_directory);

    if (!std::filesystem::exists(l_dirPath, l_ec) || !std::filesystem::is_directory(l_dirPath, l_ec)) {
        return;
    }

    for (const auto& l_entry : std::filesystem::directory_iterator(l_dirPath, std::filesystem::directory_options::skip_permission_denied, l_ec)) {
        std::filesystem::path l_entryPath = l_entry.path();

        // Skip symlinks
        if (std::filesystem::is_symlink(l_entry.path(), l_ec)) {
            continue;
        }

        // Skip pruned paths
        if (IsPrunedPath(l_entryPath)) {
            continue;
        }

        if (l_entry.is_regular_file(l_ec)) {
            if (std::filesystem::file_size(l_entry.path(), l_ec) > 0) {
                m_filesToScan.push_back(l_entryPath);
            }
        } else if (l_entry.is_directory(l_ec)) {
            m_foldersFound.push_back(l_entryPath);
            CollectFiles(l_entryPath);
        }
    }
}

std::string Scanner::ComputeSha256(const std::filesystem::path& p_filePath) {
    std::filesystem::path l_path(p_filePath);
    std::ifstream l_file(l_path, std::ios::binary);

    if (!l_file.is_open()) {
        m_config.LogError("Cannot open file: " + p_filePath.string() +
                 " (" + std::strerror(errno) + ")");
        return std::string();
    }

    SHA256_CTX l_ctx;
    SHA256_Init(&l_ctx);

    const size_t l_bufferSize = 1 << 16; // 64KB
    std::vector<char> l_buffer(l_bufferSize);
    while (l_file.read(l_buffer.data(), l_buffer.size()) || l_file.gcount()) {
        SHA256_Update(&l_ctx, l_buffer.data(), l_file.gcount());
    }

    unsigned char l_hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(l_hash, &l_ctx);

    std::ostringstream l_oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        l_oss << std::hex << std::setw(2) << std::setfill('0') << (int)l_hash[i];
    }
    return l_oss.str();
}

bool Scanner::CheckExtension(const std::filesystem::path& p_filePath) {
    return (p_filePath.extension() == ".rggJ3pSi_l"); //Qilin extension
}

void Scanner::ScanFiles() {
    size_t l_total = m_filesToScan.size();
    if (l_total == 0) {
        m_config.LogError("No files to scan.");
        return;
    }

    // Fill the task queue
    {
        std::lock_guard<std::mutex> t_lock(m_queueMutex);
        for (const auto& t_file : m_filesToScan) {
            m_taskQueue.push(t_file);
        }
    }

    // Determine number of threads to use (min 2)
    unsigned int l_numThreads = std::max(2u, std::thread::hardware_concurrency());

    std::vector<std::thread> m_workers;
    for (unsigned int i = 0; i < l_numThreads; i++) {
        m_workers.emplace_back(&Scanner::WorkerThread, this, l_total);
    }

    // Wait until all tasks are done
    {
        std::unique_lock<std::mutex> t_lock(m_queueMutex);
        while (!m_taskQueue.empty()) {
            m_cv.wait(t_lock);
        }
        m_stopThreads = true;
    }
    m_cv.notify_all();

    // Wait for all threads to finish
    for (auto& t : m_workers) {
        t.join();
    }

    std::cout << "\nAnalysis done. Please refer to "
              << m_config.Output_file() << " and " << m_config.Error_file() << " for more details.\n";

    if (std::filesystem::file_size(m_config.Output_file()) > 0) {
        std::cout << "❌ IOCs have been found on this target ❌ -> See " << m_config.Output_file() << "\n";
    } else {
        std::cout << "✅ No IOC hash found ✅\n";
    }
}

void Scanner::WorkerThread(size_t total) {
    while (true) {
        std::filesystem::path l_file;

        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            if (m_taskQueue.empty()) {
                if (m_stopThreads) break;
                m_cv.wait(lock);
                continue;
            }
            l_file = m_taskQueue.front();
            m_taskQueue.pop();

            if (m_taskQueue.empty()) {
                m_cv.notify_all(); // prévenir quand la queue est vide
            }
        }

        ScanFileTask(l_file, total);
    }
}

void Scanner::ScanFileTask(const std::filesystem::path& p_file, size_t p_total) {
    try {
        std::string l_hash = ComputeSha256(p_file);
        if (!l_hash.empty() && m_iocHashes.find(l_hash) != m_iocHashes.end()) {
            std::lock_guard<std::mutex> lock(m_outputMutex);
            m_config.LogOutput("Correspondence found: " + p_file.string() + " (hash: " + l_hash + ")");
        }
        if (CheckExtension(p_file))
        {
            std::lock_guard<std::mutex> lock(m_outputMutex);
            m_config.LogOutput("Extension found: " + p_file.string());
        }
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock(m_errorMutex);
        m_config.LogError("Error reading file: " + p_file.string() + " (" + e.what() + ")");
    }

    {
        std::lock_guard<std::mutex> lock(m_progressMutex);
        m_progressCount++;
        PrintProgress(m_progressCount, p_total);
    }
}

void Scanner::PrintProgress(size_t p_current, size_t p_total) {
    const int l_barSize = 50;
    int l_percent = static_cast<int>(p_current * 100 / p_total);
    int l_filled = l_percent * l_barSize / 100;

    std::cout << "\r[";
    for (int i = 0; i < l_filled; i++) std::cout << "#";
    for (int i = l_filled; i < l_barSize; i++) std::cout << " ";
    std::cout << "] " << l_percent << "% (" << p_current << "/" << p_total << ")";
    std::cout.flush();
}

bool Scanner::IsPrunedPath(const std::filesystem::path& p_path) {
    auto abs_path = std::filesystem::weakly_canonical(p_path);

    for (const auto& pruned : m_config.Prune_paths()) {
        auto abs_pruned = std::filesystem::weakly_canonical(pruned);

        if (!abs_path.lexically_relative(abs_pruned).empty()) {
            return true;
        }
    }
    return false;
}