#pragma once

#include "SettingsCommon.h"
#include "../../page_manager/Page.h"
#include "../../ui_kit/Button.h"
#include "../../ui_kit/FrameLayout.h"
#include "../../ui_kit/LinearLayout.h"
#include "../../ui_kit/TextView.h"

#include <vector>

class SettingsPage : public Page {
public:
    SettingsPage();
    ~SettingsPage() override;

    void onCreate() override;
    void onStart() override;
    void onResume() override;
    void onPause() override;
    void onStop() override;
    void onDestroy() override;

private:
    struct SettingItemBinding {
        SettingKey key;
        TextView* titleView;
        TextView* valueView;
    };

    void addSettingItem(SettingKey key);
    void refreshConfigValues();
    void refreshStaticTexts();

    FrameLayout* _layout = nullptr;
    LinearLayout* _rootContainer = nullptr;
    LinearLayout* _headerRow = nullptr;
    LinearLayout* _listContainer = nullptr;
    Button* _backButton = nullptr;
    TextView* _titleView = nullptr;
    std::vector<SettingItemBinding> _itemBindings;
};
