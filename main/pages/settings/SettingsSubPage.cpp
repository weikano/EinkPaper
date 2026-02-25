#include "SettingsSubPage.h"

#include "../../config/DeviceConfigManager.h"
#include "../../page_manager/PageManager.h"
#include "esp_log.h"

#include <memory>

static const char* TAG = "SettingsSubPage";

class SettingsSubPage::OptionItemView : public View {
public:
    OptionItemView(int16_t width, int16_t height, const std::string& text)
        : View(width, height), _text(text) {}

    void setSelected(bool selected) {
        if (_selected != selected) {
            _selected = selected;
            markDirty();
        }
    }

    std::string className() const override { return "SettingsOptionItem"; }

protected:
    void onDraw(lgfx::LovyanGFX& display) override {
        if (_visibility == GONE) {
            return;
        }

        const uint32_t background = _selected ? TFT_BLACK : TFT_WHITE;
        const uint32_t textColor = _selected ? TFT_WHITE : TFT_BLACK;

        display.fillRect(_left, _top, _width, _height, background);
        display.drawRect(_left, _top, _width, _height, TFT_BLACK);

        display.setTextColor(textColor);
        display.setTextSize(1);

        int16_t textX = _left + 12;
        int16_t textY = _top + (_height - display.fontHeight()) / 2;
        display.setCursor(textX, textY);
        display.print(_text.c_str());
    }

private:
    std::string _text;
    bool _selected = false;
};

SettingsSubPage::SettingsSubPage() : Page(PageType::SETTINGS_SUB, "SettingsSubPage") {
    ESP_LOGI(TAG, "SettingsSubPage constructed");
}

SettingsSubPage::~SettingsSubPage() {
    ESP_LOGI(TAG, "SettingsSubPage destructed");
}

void SettingsSubPage::onCreate() {
    ESP_LOGI(TAG, "SettingsSubPage onCreate");

    if (auto params = std::static_pointer_cast<SettingsSubPageParams>(getParams())) {
        _settingKey = params->settingKey;
    }

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
    _titleView->setText(getSettingTitle(_settingKey));
    _titleView->setTextAlign(1);

    _headerRow->addChild(_backButton);
    _headerRow->addChild(_titleView);

    _optionsContainer = new LinearLayout(MATCH_PARENT, MATCH_PARENT, LinearLayout::Orientation::VERTICAL);
    _optionsContainer->setSpacing(8);

    const auto options = getSettingOptions(_settingKey);
    for (const auto& option : options) {
        auto* optionView = new OptionItemView(MATCH_PARENT, 62, option.second);
        optionView->setOnClickListener([this, option]() { onOptionSelected(option.first); });
        _optionsContainer->addChild(optionView);
        _optionBindings.push_back({option.first, optionView});
    }

    _rootContainer->addChild(_headerRow);
    _rootContainer->addChild(_optionsContainer);
    _layout->addChild(_rootContainer);

    setRootView(_layout);
    refreshSelection();

    Page::onCreate();
}

void SettingsSubPage::onStart() {
    ESP_LOGI(TAG, "SettingsSubPage onStart");
    Page::onStart();
}

void SettingsSubPage::onResume() {
    ESP_LOGI(TAG, "SettingsSubPage onResume");
    refreshSelection();
    Page::onResume();
}

void SettingsSubPage::onPause() {
    ESP_LOGI(TAG, "SettingsSubPage onPause");
    Page::onPause();
}

void SettingsSubPage::onStop() {
    ESP_LOGI(TAG, "SettingsSubPage onStop");
    Page::onStop();
}

void SettingsSubPage::onDestroy() {
    ESP_LOGI(TAG, "SettingsSubPage onDestroy");

    _optionBindings.clear();
    _titleView = nullptr;
    _backButton = nullptr;
    _optionsContainer = nullptr;
    _headerRow = nullptr;
    _rootContainer = nullptr;
    _layout = nullptr;

    Page::onDestroy();
}

void SettingsSubPage::onOptionSelected(int value) {
    auto& manager = DeviceConfigManager::getInstance();
    DeviceConfig config = manager.getConfig();

    applySettingValue(config, _settingKey, value);
    manager.setConfig(config);
    manager.saveConfigToSdCard();

    refreshSelection();
}

void SettingsSubPage::refreshSelection() {
    const auto& config = DeviceConfigManager::getInstance().getConfig();

    int selectedValue = 0;
    switch (_settingKey) {
        case SettingKey::Language:
            selectedValue = static_cast<int>(config.language);
            break;
        case SettingKey::RefreshMode:
            selectedValue = static_cast<int>(config.refreshMode);
            break;
        case SettingKey::FontSize:
            selectedValue = static_cast<int>(config.fontSize);
            break;
        case SettingKey::AutoSleep:
            selectedValue = static_cast<int>(config.autoSleep);
            break;
        default:
            selectedValue = 0;
            break;
    }

    for (auto& binding : _optionBindings) {
        if (binding.view) {
            binding.view->setSelected(binding.value == selectedValue);
        }
    }
}
