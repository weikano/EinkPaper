#pragma once

#include "../../page_manager/Page.h"
#include "../../ui_kit/View.h"
#include "../../ui_kit/ViewGroup.h"

class SleepPage : public Page {
public:
    SleepPage();
    ~SleepPage() override;
    static bool consumeWakeupRequest();

    void onCreate() override;
    void onResume() override;
    void onDestroy() override;

private:
    class BlackView : public View {
    public:
        BlackView(int16_t width, int16_t height) : View(width, height) {}
        std::string className() const override { return "SleepBlackView"; }

    protected:
        void onDraw(lgfx::LovyanGFX& display) override {
            display.fillRect(getLeft(), getTop(), getWidth(), getHeight(), TFT_BLACK);
        }
    };

    ViewGroup* _layout = nullptr;
    BlackView* _blackView = nullptr;
    static bool _wakeupRequested;
};
