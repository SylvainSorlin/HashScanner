#pragma once
#include <string>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <vector>

class Config {
public:
    explicit Config(const std::filesystem::path& filename);

    //GETTERS
    const std::filesystem::path& Output_file() const { return m_outputFile; }
    const std::filesystem::path& Error_file() const { return m_errorFile; }
    const std::filesystem::path& Scan_dir() const { return m_scanDir; }
    const std::filesystem::path& Ioc_file() const { return m_iocFile; }
    const std::vector<std::filesystem::path>& Prune_paths() const { return m_prunePaths; }

    //LOG
    void LogOutput(const std::string& message);
    void LogError(const std::string& message);

private:
    std::filesystem::path m_scanDir;
    std::filesystem::path m_outputFile;
    std::filesystem::path m_errorFile;
    std::filesystem::path m_iocFile;
    std::vector<std::filesystem::path> m_prunePaths;

    static std::string GetIniValue(std::ifstream& ini, const std::string& key);
    static std::string Trim(const std::string& s);
    void Validate();
    void PrintInfo() const;
    void ParsePrunePaths(const std::string& pruneStr);
};
