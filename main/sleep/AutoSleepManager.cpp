#include "AutoSleepManager.h"

#include "../config/DeviceConfigManager.h"
#include "../page_manager/PageManager.h"
#include "esp_timer.h"

void AutoSleepManager::init() {
    _lastActivityMs = esp_timer_get_time() / 1000;
}

void AutoSleepManager::notifyUserActivity() {
    _lastActivityMs = esp_timer_get_time() / 1000;
}

void AutoSleepManager::notifyWakeFromSleep() {
    _lastActivityMs = esp_timer_get_time() / 1000;
}

int64_t AutoSleepManager::getAutoSleepTimeoutMs() const {
    return AutoSleepDuration::getAutoSleepDurationInMS(
        DeviceConfigManager::getInstance().getConfig().autoSleep);
}

void AutoSleepManager::update() {
    Page* currentPage = PageManager::getInstance().getCurrentPage();
    if (!currentPage) {
        return;
    }
    if (currentPage->getType() == PageType::SLEEP) {
        return;
    }
    if (currentPage->preventAutoSleep()) {
        return;
    }

    const int64_t timeoutMs = getAutoSleepTimeoutMs();
    if (timeoutMs <= 0) {
        return;
    }

    const int64_t nowMs = esp_timer_get_time() / 1000;
    if ((nowMs - _lastActivityMs) >= timeoutMs) {
        PageManager::getInstance().startActivity(PageType::SLEEP);
        _lastActivityMs = nowMs;
    }
}
