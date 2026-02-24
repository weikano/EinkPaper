#pragma once

#include <esp_err.h>
#include <string>

struct OtaCheckResult {
    bool updateAvailable = false;
    bool force = false;
    int latestCommitCount = 0;
    std::string url;
    std::string sha256;
    std::string message;
};

class OtaService {
public:
    static esp_err_t checkForUpdate(const std::string& endpoint, OtaCheckResult& outResult);
    static esp_err_t downloadAndInstall(const OtaCheckResult& result, std::string& errMsg);
    static std::string getCurrentVersionString();
    static int getCurrentCommitCount();
    static std::string getDeviceType();
};

