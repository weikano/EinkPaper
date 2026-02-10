#include "QRCodeView.h"
#include "esp_log.h"

static const char* TAG = "QRCodeView";

QRCodeView::QRCodeView(int16_t width, int16_t height)
    : View(width, height)
{
    ESP_LOGD(TAG, "QRCodeView created with width %d and height %d", width, height);
}

QRCodeView::~QRCodeView()
{
    ESP_LOGD(TAG, "QRCodeView destroyed");
}

void QRCodeView::setQRCode(const std::string& qrcode)
{
    if(_qrcode != qrcode) {
        _qrcode = qrcode;
        markDirty();
    }    
}

void QRCodeView::onDraw(m5gfx::M5GFX& display)
{
    if(_qrcode.empty()) {
        return;
    }
    M5.Display.qrcode(_qrcode.c_str(), getLeft(), getTop(), getWidth(), 0, true);
}

void QRCodeView::measure(int16_t widthMeasureSpec, int16_t heightMeasureSpec)
{
    // 如果宽度或高度未设置，可以根据文本内容计算
    if (_width <= 0) {
        if (widthMeasureSpec > 0) {
            _width = widthMeasureSpec;
        }
    }
    
    if (_height <= 0) {
        if (heightMeasureSpec > 0) {
            _height = heightMeasureSpec;
        }
    }
    ESP_LOGD(TAG, "QRCodeView measure with width %d and height %d", _width, _height);
}