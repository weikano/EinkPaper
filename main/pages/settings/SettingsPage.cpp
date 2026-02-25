#include "SettingsPage.h"

#include "../../config/DeviceConfigManager.h"
#include "../../page_manager/PageManager.h"
#include "../../page_manager/PageType.h"
#include "esp_log.h"

#include <memory>

static const char* TAG = "SettingsPage";

SettingsPage::SettingsPage() : Page(PageType::SETTINGS, "Settings") {
    ESP_LOGI(TAG, "SettingsPage constructed");
}

SettingsPage::~SettingsPage() {
    ESP_LOGI(TAG, "SettingsPage destructed");
}

void SettingsPage::onCreate() {
    ESP_LOGI(TAG, "SettingsPage onCreate");

    const int16_t screenWidth = M5.Display.width();
    const int16_t screenHeight = M5.Display.height();

    _layout = new FrameLayout(screenWidth, screenHeight);

    _rootContainer = new LinearLayout(MATCH_PARENT, MATCH_PARENT, LinearLayout::Orientation::VERTICAL);
    _rootContainer->setPadding(12, 12, 12, 12);
    _rootContainer->setSpacing(10);

    _headerRow = new LinearLayout(MATCH_PARENT, 72, LinearLayout::Orientation::HORIZONTAL);
    _headerRow->setSpacing(10);

    _backButton = new Button(96, 56);
    _backButton->setText("返回");
    _backButton->setOnClickListener([]() { PageManager::getInstance().goBack(); });

    _titleView = new TextView(screenWidth - 130, 56);
    _titleView->setText("设置");
    _titleView->setTextAlign(1);

    _headerRow->addChild(_backButton);
    _headerRow->addChild(_titleView);

    _listContainer = new LinearLayout(MATCH_PARENT, MATCH_PARENT, LinearLayout::Orientation::VERTICAL);
    _listContainer->setSpacing(10);

    addSettingItem(SettingKey::Language);
    addSettingItem(SettingKey::RefreshMode);
    addSettingItem(SettingKey::FontSize);
    addSettingItem(SettingKey::AutoSleep);

    _rootContainer->addChild(_headerRow);
    _rootContainer->addChild(_listContainer);
    _layout->addChild(_rootContainer);

    setRootView(_layout);
    refreshConfigValues();

    Page::onCreate();
}

void SettingsPage::onStart() {
    ESP_LOGI(TAG, "SettingsPage onStart");
    Page::onStart();
}

void SettingsPage::onResume() {
    ESP_LOGI(TAG, "SettingsPage onResume");
    refreshConfigValues();
    Page::onResume();
}

void SettingsPage::onPause() {
    ESP_LOGI(TAG, "SettingsPage onPause");
    Page::onPause();
}

void SettingsPage::onStop() {
    ESP_LOGI(TAG, "SettingsPage onStop");
    Page::onStop();
}

void SettingsPage::onDestroy() {
    ESP_LOGI(TAG, "SettingsPage onDestroy");

    _itemBindings.clear();
    _titleView = nullptr;
    _backButton = nullptr;
    _listContainer = nullptr;
    _headerRow = nullptr;
    _rootContainer = nullptr;
    _layout = nullptr;

    Page::onDestroy();
}

void SettingsPage::addSettingItem(SettingKey key) {
    auto* itemLayout = new LinearLayout(MATCH_PARENT, 86, LinearLayout::Orientation::VERTICAL);
    itemLayout->setPadding(12, 8, 12, 8);
    itemLayout->setSpacing(4);
    itemLayout->setBorderWidth(1);

    auto* titleView = new TextView(MATCH_PARENT, 34);
    titleView->setText(getSettingTitle(key));

    auto* valueView = new TextView(MATCH_PARENT, 30);
    valueView->setText("-");

    auto openSubPage = [key]() {
        auto params = std::make_shared<SettingsSubPageParams>();
        params->settingKey = key;
        PageManager::getInstance().startActivity(PageType::SETTINGS_SUB, params);
    };

    itemLayout->setOnClickListener(openSubPage);
    titleView->setOnClickListener(openSubPage);
    valueView->setOnClickListener(openSubPage);

    itemLayout->addChild(titleView);
    itemLayout->addChild(valueView);
    _listContainer->addChild(itemLayout);

    _itemBindings.push_back({key, valueView});
}

void SettingsPage::refreshConfigValues() {
    const auto& config = DeviceConfigManager::getInstance().getConfig();
    for (auto& binding : _itemBindings) {
        if (binding.valueView) {
            binding.valueView->setText(getSettingCurrentValueText(binding.key, config));
        }
    }
}
