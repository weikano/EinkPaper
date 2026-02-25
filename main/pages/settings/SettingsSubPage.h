#pragma once

#include "SettingsCommon.h"
#include "../../page_manager/Page.h"
#include "../../ui_kit/Button.h"
#include "../../ui_kit/FrameLayout.h"
#include "../../ui_kit/LinearLayout.h"
#include "../../ui_kit/TextView.h"
#include "../../ui_kit/View.h"

#include <vector>

class SettingsSubPage : public Page {
public:
    SettingsSubPage();
    ~SettingsSubPage() override;

    void onCreate() override;
    void onStart() override;
    void onResume() override;
    void onPause() override;
    void onStop() override;
    void onDestroy() override;

private:
    class OptionItemView;

    struct OptionViewBinding {
        int value;
        OptionItemView* view;
    };

    void onOptionSelected(int value);
    void refreshTexts();
    void refreshSelection();

    FrameLayout* _layout = nullptr;
    LinearLayout* _rootContainer = nullptr;
    LinearLayout* _headerRow = nullptr;
    LinearLayout* _optionsContainer = nullptr;
    Button* _backButton = nullptr;
    TextView* _titleView = nullptr;

    SettingKey _settingKey = SettingKey::Language;
    std::vector<OptionViewBinding> _optionBindings;
};
