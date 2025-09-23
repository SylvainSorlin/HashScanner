#include "Config.h"
#include <cstring>
#include <cerrno>

Config::Config(const std::filesystem::path& p_filename) {
    std::ifstream ini(p_filename, std::ios::binary);
    if (!ini.is_open()) {
        throw std::runtime_error("Cannot open file '" + p_filename.string() + "' (" + std::strerror(errno) + ")");
    }

    m_scanDir    = GetIniValue(ini, "scan_dir");
    m_outputFile = GetIniValue(ini, "output_file");
    m_errorFile  = GetIniValue(ini, "error_file");
    m_iocFile    = GetIniValue(ini, "ioc_file");
    ParsePrunePaths(GetIniValue(ini, "prune_paths"));
    std::istringstream(GetIniValue(ini, "debug")) >> m_debug;

    Validate();
    PrintInfo();
}

void Config::ParsePrunePaths(const std::string& p_pruneStr) {
    m_prunePaths.clear();
    std::stringstream ss(p_pruneStr);
    std::string item;
    while (std::getline(ss, item, ';')) {
        if (!item.empty()) {
            m_prunePaths.push_back(item);
        }
    }
}

std::string Config::Trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos || end == std::string::npos)
        return "";
    return s.substr(start, end - start + 1);
}

std::string Config::GetIniValue(std::ifstream& p_ini, const std::string& p_key) {
    p_ini.clear();
    p_ini.seekg(0);
    std::string line;
    while (std::getline(p_ini, line)) {
        size_t eq = line.find('=');
        if (eq != std::string::npos) {
            std::string k = Trim(line.substr(0, eq));
            std::string v = Trim(line.substr(eq + 1));
            if (k == p_key) return v;
        }
    }
    return "";
}

void Config::PrintInfo() const {
    std::cout << "Config Info:" << std::endl;
    std::cout << "  scanDir    : " << m_scanDir.string() << std::endl;
    std::cout << "  outputFile : " << m_outputFile.string() << std::endl;
    std::cout << "  errorFile  : " << m_errorFile.string() << std::endl;
    std::cout << "  iocFile    : " << m_iocFile.string() << std::endl;
    std::cout << "  debug      : " << m_debug << std::endl;

    std::cout << "  prunePaths :" << std::endl;
    if (m_prunePaths.empty()) {
        std::cout << "    (none)" << std::endl;
    } else {
        for (const auto& path : m_prunePaths) {
            std::cout << "    " << path.string() << std::endl;
        }
    }
}

void Config::Validate() {
    bool missing = false;
    if (m_scanDir.empty())    { LogError("Missing config value: scan_dir");    missing = true; }
    if (m_outputFile.empty()) { LogError("Missing config value: output_file"); missing = true; }
    if (m_errorFile.empty())  { LogError("Missing config value: error_file");  missing = true; }
    if (m_iocFile.empty())    { LogError("Missing config value: ioc_file");    missing = true; }
    if (m_prunePaths.empty()) { LogError("Warning: prune_paths not set."); }

    if (missing) throw std::runtime_error("Please check your config file.");
}

// LOG
void Config::LogOutput(const std::string& p_message) {
    std::ofstream out(m_outputFile, std::ios::app);
    out << p_message << std::endl;
}

void Config::LogError(const std::string& p_message) {
    std::ofstream err(m_errorFile, std::ios::app);
    err << p_message << std::endl;
    if(m_debug)
    {
        std::cerr << p_message << std::endl;
    }
}