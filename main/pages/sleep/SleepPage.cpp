#include "SleepPage.h"

#include "../../sleep/AutoSleepManager.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sleep.h"

static const char* TAG = "SleepPage";
static constexpr gpio_num_t kWakeTouchPinPaperS3 = GPIO_NUM_48;
bool SleepPage::_wakeupRequested = false;

static bool enterPlatformSleep() {
    if (M5.getBoard() == m5::board_t::board_M5PaperS3) {
        ESP_ERROR_CHECK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL));
        ESP_ERROR_CHECK(gpio_wakeup_enable(kWakeTouchPinPaperS3, GPIO_INTR_LOW_LEVEL));
        ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup());

        ESP_LOGI(TAG, "Enter light sleep (touch wake on GPIO48)");
        ESP_ERROR_CHECK(esp_light_sleep_start());

        ESP_ERROR_CHECK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_GPIO));
        ESP_LOGI(TAG, "Wake from light sleep by touch");
        return true;
    }

    ESP_ERROR_CHECK(esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL));
    constexpr gpio_num_t kWakeButtonPin = GPIO_NUM_0;
    ESP_ERROR_CHECK(esp_sleep_enable_ext1_wakeup_io(
        (1ULL << static_cast<uint8_t>(kWakeButtonPin)),
        ESP_EXT1_WAKEUP_ANY_LOW));

    ESP_LOGI(TAG, "Enter deep sleep now");
    esp_deep_sleep_start();
    return false;
}

SleepPage::SleepPage() : Page(PageType::SLEEP, "SleepPage") {}

SleepPage::~SleepPage() = default;

bool SleepPage::consumeWakeupRequest() {
    const bool requested = _wakeupRequested;
    _wakeupRequested = false;
    return requested;
}

void SleepPage::onCreate() {
    const int16_t screenWidth = M5.Display.width();
    const int16_t screenHeight = M5.Display.height();

    _layout = new ViewGroup(screenWidth, screenHeight);
    _blackView = new BlackView(screenWidth, screenHeight);
    _blackView->setPosition(0, 0);
    _layout->addChild(_blackView);
    setRootView(_layout);

    Page::onCreate();
}

void SleepPage::onResume() {
    Page::onResume();

    M5.Display.startWrite();
    M5.Display.fillScreen(TFT_BLACK);
    M5.Display.endWrite();
    M5.Display.display();
    M5.Display.waitDisplay();
    M5.Display.sleep();

    const bool resumed = enterPlatformSleep();
    if (resumed) {
        M5.Display.wakeup();
        M5.Display.waitDisplay();
        AutoSleepManager::getInstance().notifyWakeFromSleep();
        _wakeupRequested = true;
    }
}

void SleepPage::onDestroy() {
    _layout = nullptr;
    _blackView = nullptr;
    Page::onDestroy();
}
