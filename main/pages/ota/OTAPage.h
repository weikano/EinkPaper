#pragma once

#include "../../page_manager/Page.h"
#include "../../ui_kit/Button.h"
#include "../../ui_kit/TextView.h"
#include "../../ui_kit/ViewGroup.h"

class OTAPage : public Page {
public:
    OTAPage();
    ~OTAPage();

    void onCreate() override;
    void onDestroy() override;

private:
    void startOtaFlow();
    void setVersionText(const std::string& text);

    ViewGroup* _layout = nullptr;
    TextView* _versionText = nullptr;
    Button* _otaButton = nullptr;
    bool _isUpdating = false;
};
