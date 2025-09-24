#ifndef YARASCANNER_H
#define YARASCANNER_H

#include <string>
#include <vector>
#include <filesystem>
#include "Config.h"

extern "C" {
#include <yara.h>
}

class YaraScanner {
public:
    YaraScanner(const Config& p_Config);
    ~YaraScanner();

    bool Initialize(const std::filesystem::path& p_rulesFile);

    bool ScanFile(const std::filesystem::path& p_filePath, std::vector<std::string>& p_matchedRules);

private:
    Config m_config;
    YR_COMPILER* m_compiler;
    YR_RULES* m_rules;

    static int CallbackFunction(YR_SCAN_CONTEXT* p_context, int p_message, void* p_message_data, void* p_user_data);
};

#endif // YARASCANNER_H