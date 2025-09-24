#include "YaraScanner.h"
#include <iostream>
#include <cstdio>

YaraScanner::YaraScanner(const Config& p_Config) : m_config(p_Config), m_compiler(nullptr), m_rules(nullptr) {
    if (yr_initialize() != ERROR_SUCCESS) {
        m_config.LogError("[YARA] Initialization error!");
    }
}

YaraScanner::~YaraScanner() {
    if (m_rules) yr_rules_destroy(m_rules);
    if (m_compiler) yr_compiler_destroy(m_compiler);
    yr_finalize();
}

bool YaraScanner::Initialize(const std::filesystem::path& p_rulesFile) {
    if (yr_compiler_create(&m_compiler) != ERROR_SUCCESS) {
        m_config.LogError("[YARA] Failed to create compiler!");
        return false;
    }

    FILE* f = fopen(p_rulesFile.string().c_str(), "r");
    if (!f) {
        m_config.LogError("[YARA] Failed to open rules file: "+p_rulesFile.string());
        return false;
    }

    if (yr_compiler_add_file(m_compiler, f, nullptr, p_rulesFile.string().c_str()) != 0) {
        m_config.LogError("[YARA] Error compiling rules!");
        fclose(f);
        return false;
    }

    fclose(f);

    if (yr_compiler_get_rules(m_compiler, &m_rules) != ERROR_SUCCESS) {
        m_config.LogError("[YARA] Failed to get compiled rules!");
        return false;
    }

    return true;
}

int YaraScanner::CallbackFunction(YR_SCAN_CONTEXT* p_context, int p_message, void* p_message_data, void* p_user_data) {
    if (p_message == CALLBACK_MSG_RULE_MATCHING) {
        YR_RULE* rule = (YR_RULE*) p_message_data;
        auto matches = reinterpret_cast<std::vector<std::string>*>(p_user_data);
        matches->push_back(rule->identifier);
    }
    return CALLBACK_CONTINUE;
}

bool YaraScanner::ScanFile(const std::filesystem::path& p_filePath, std::vector<std::string>& p_matchedRules) {
    if (!m_rules) {
        m_config.LogError("[YARA] No rules loaded!");
        return false;
    }

    int res = yr_rules_scan_file(m_rules, p_filePath.string().c_str(), 0, CallbackFunction, &p_matchedRules, 0);
    return (res == ERROR_SUCCESS && !p_matchedRules.empty());
}
