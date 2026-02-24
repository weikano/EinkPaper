#include "OTAPage.h"

#include "../../hal/ota/OtaService.h"
#include "esp_log.h"
#include "esp_system.h"

static const char* TAG = "OTAPage";
static const char* OTA_CHECK_URL = "https://www.baidu.com";

OTAPage::OTAPage() : Page(PageType::OTA, "OTAPage") {}

OTAPage::~OTAPage() = default;

void OTAPage::onCreate() {
    const int16_t screenWidth = M5.Display.width();
    const int16_t screenHeight = M5.Display.height();

    _layout = new ViewGroup(screenWidth, screenHeight);

    _versionText = new TextView(screenWidth - 40, 80);
    _versionText->setTextAlign(1);
    _versionText->setTextSize(1);
    _versionText->setPadding(8, 8, 8, 8);

    _otaButton = new Button(220, 60);
    _otaButton->setText("检查并升级");
    _otaButton->setOnClickListener([this]() { startOtaFlow(); });

    const int16_t gap = 20;
    const int16_t totalHeight = _versionText->getHeight() + gap + _otaButton->getHeight();
    const int16_t startY = (screenHeight - totalHeight) / 2;

    _versionText->setPosition((screenWidth - _versionText->getWidth()) / 2, startY);
    _otaButton->setPosition((screenWidth - _otaButton->getWidth()) / 2, startY + _versionText->getHeight() + gap);

    setVersionText("当前版本: " + OtaService::getCurrentVersionString());

    _layout->addChild(_versionText);
    _layout->addChild(_otaButton);
    setRootView(_layout);

    Page::onCreate();
}

void OTAPage::onDestroy() {
    _layout = nullptr;
    _versionText = nullptr;
    _otaButton = nullptr;
    _isUpdating = false;
    Page::onDestroy();
}

void OTAPage::setVersionText(const std::string& text) {
    if (_versionText) {
        _versionText->setText(text);
    }
    if (_layout) {
        _layout->forceRedraw();
    }
}

void OTAPage::startOtaFlow() {
    if (_isUpdating) {
        ESP_LOGW(TAG, "OTA is already running");
        return;
    }

    _isUpdating = true;
    setVersionText("版本: " + OtaService::getCurrentVersionString() + "\n正在检测更新...");

    OtaCheckResult checkResult;
    esp_err_t err = OtaService::checkForUpdate(OTA_CHECK_URL, checkResult);
    if (err != ESP_OK) {
        setVersionText("OTA检测失败\n" + checkResult.message);
        _isUpdating = false;
        return;
    }

    if (!checkResult.updateAvailable) {
        setVersionText("当前已是最新版本\n" + OtaService::getCurrentVersionString());
        _isUpdating = false;
        return;
    }

    setVersionText("发现新版本: " + std::to_string(checkResult.latestCommitCount) + "\n开始下载并校验...");

    std::string otaErr;
    err = OtaService::downloadAndInstall(checkResult, otaErr);
    if (err != ESP_OK) {
        setVersionText("OTA失败\n" + otaErr);
        _isUpdating = false;
        return;
    }

    setVersionText("OTA成功\n即将重启");
    ESP_LOGI(TAG, "OTA success, reboot now");
    esp_restart();
}
