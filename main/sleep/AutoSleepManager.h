#pragma once

#include <cstdint>

class AutoSleepManager {
public:
    static AutoSleepManager& getInstance() {
        static AutoSleepManager instance;
        return instance;
    }

    void init();
    void notifyUserActivity();
    void notifyWakeFromSleep();
    void update();

private:
    AutoSleepManager() = default;

    int64_t getAutoSleepTimeoutMs() const;

    int64_t _lastActivityMs = 0;
};
