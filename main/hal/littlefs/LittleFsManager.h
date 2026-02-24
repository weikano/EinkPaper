#pragma once

#include "esp_err.h"

class LittleFsManager {
public:
    static LittleFsManager& getInstance() {
        static LittleFsManager instance;
        return instance;
    }

    esp_err_t mount();
    void unmount();
    bool isMounted() const { return _mounted; }

private:
    LittleFsManager() = default;
    bool _mounted = false;
};

