#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "M5Unified.h"
#include "ui_kit/UIKIT.h"

// 全局变量
m5gfx::M5GFX& display = M5.Display;

// UI组件
LinearLayout* mainLayout = nullptr;
TextView* titleLabel = nullptr;
Button* button1 = nullptr;
Button* button2 = nullptr;
ListView* listView = nullptr;
Dialog* dialog = nullptr;

// 按钮点击回调
void onButton1Click() {
    printf("Button 1 clicked!\n");
    
    // 创建并显示对话框
    if (dialog) {
        dialog->setTitle("提示");
        dialog->setMessage("这是第一个按钮的点击事件！");
        dialog->setButton(Dialog::BUTTON_POSITIVE, "确定");
        dialog->setButton(Dialog::BUTTON_NEGATIVE, "取消");
        dialog->show();
    }
}

void onButton2Click() {
    printf("Button 2 clicked!\n");
    
    // 创建并显示对话框
    if (dialog) {
        dialog->setTitle("信息");
        dialog->setMessage("这是第二个按钮的点击事件！");
        dialog->setButton(Dialog::BUTTON_POSITIVE, "好的");
        dialog->show();
    }
}

void onListItemClick(int index) {
    printf("List item %d clicked!\n", index);
    
    if (dialog) {
        dialog->setTitle("列表项点击");
        dialog->setMessage("你点击了第 " + std::to_string(index + 1) + " 项");
        dialog->setButton(Dialog::BUTTON_POSITIVE, "确定");
        dialog->show();
    }
}

void setupUI() {
    // 创建根布局
    mainLayout = new LinearLayout(0, 0, display.width(), display.height(), LinearLayout::VERTICAL);
    mainLayout->setSpacing(10);
    
    // 创建标题
    titleLabel = new TextView(10, 10, display.width() - 20, 40);
    titleLabel->setText("M5GFX UI Kit 示例");
    titleLabel->setTextColor(TFT_BLUE);
    titleLabel->setTextSize(2);
    titleLabel->setTextAlign(1); // 居中对齐
    titleLabel->setBackgroundColor(TFT_LIGHTGREY);
    mainLayout->addChild(titleLabel);
    
    // 创建按钮布局
    LinearLayout* buttonLayout = new LinearLayout(10, 60, display.width() - 20, 60, LinearLayout::HORIZONTAL);
    buttonLayout->setSpacing(10);
    
    // 创建按钮1
    button1 = new Button(0, 0, (display.width() - 40) / 2, 40);
    button1->setText("按钮 1");
    button1->setOnClickListener(onButton1Click);
    buttonLayout->addChild(button1);
    
    // 创建按钮2
    button2 = new Button(0, 0, (display.width() - 40) / 2, 40);
    button2->setText("按钮 2");
    button2->setOnClickListener(onButton2Click);
    buttonLayout->addChild(button2);
    
    mainLayout->addChild(buttonLayout);
    
    // 创建列表
    listView = new ListView(10, 130, display.width() - 20, 150);
    listView->setBorderColor(TFT_DARKGREY);
    listView->setBorderWidth(1);
    
    // 添加列表项
    std::vector<std::string> items = {
        "项目 1",
        "项目 2",
        "项目 3", 
        "项目 4",
        "项目 5",
        "项目 6",
        "项目 7",
        "项目 8"
    };
    listView->setItems(items);
    listView->setOnItemClickListener(onListItemClick);
    mainLayout->addChild(listView);
    
    // 创建对话框
    dialog = new Dialog(display);
    dialog->setOnButtonClickListener([](int buttonId) {
        printf("Dialog button %d clicked\n", buttonId);
    });
}

void loopUI() {
    // 更新触摸状态
    M5.update();
    auto touch = M5.Touch.getDetail(0);
    
    if (touch.wasPressed()) {
        // 处理UI触摸事件
        mainLayout->onTouch(touch.x, touch.y);
    }
    
    // 绘制UI
    display.startWrite();
    display.fillRect(0, 0, display.width(), display.height(), TFT_WHITE);
    mainLayout->draw(display);
    if (dialog->isShowing()) {
        dialog->draw(display);
    }
    display.endWrite();
    
    // 控制帧率
    vTaskDelay(pdMS_TO_TICKS(50)); // 约20FPS
}

extern "C" void ui_demo_task(void *pvParameters) {
    // 初始化UI
    setupUI();
    
    // UI主循环
    while(1) {
        loopUI();
    }
}

extern "C" void app_main(void) {
    // 初始化M5设备
    auto cfg = m5::M5Unified::config();
    M5.begin(cfg);
    M5.Display.setBrightness(128);
    
    // 设置显示模式为快速刷新（适用于电子墨水屏）
    M5.Display.setEpdMode(m5gfx::epd_mode_t::epd_fast);
    
    // 创建UI任务
    xTaskCreate(ui_demo_task, "ui_demo_task", 8192, NULL, 1, NULL);
}