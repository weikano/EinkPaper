/**
 * MessagePage 使用示例
 * 
 * MessagePage 是一个全屏文本显示页面，用于显示重要消息或通知。
 * 
 * 基本用法：
 * 
 * 1. 显示一条消息：
 *    PageManager& pm = PageManager::getInstance();
 *    pm.startActivity(PageType::MESSAGE, 
 *                     std::make_shared<std::string>("Hello, World!"));
 * 
 * 2. 更新现有消息页面的内容：
 *    MessagePage* msgPage = dynamic_cast<MessagePage*>(currentPage);
 *    if (msgPage) {
 *        msgPage->setMessage("New message");
 *    }
 * 
 * 3. 关闭消息页面：
 *    PageManager& pm = PageManager::getInstance();
 *    pm.finishActivity();
 * 
 * 特性：
 * - 全屏显示
 * - 文本居中
 * - 自适应屏幕尺寸
 * - 支持长文本自动换行
 * - 统一参数传递接口
 */

// 示例：在文件操作期间显示进度消息
void showFileOperationMessage(const std::string& operation) {
    std::string message = "正在" + operation + "...";
    
    // 创建参数并启动页面
    PageManager& pm = PageManager::getInstance();
    auto msgParam = std::make_shared<std::string>(message);
    pm.startActivity(PageType::MESSAGE, msgParam);
}

// 示例：显示错误消息
void showError(const std::string& error) {
    std::string errorMessage = "错误：" + error;
    
    PageManager& pm = PageManager::getInstance();
    auto msgParam = std::make_shared<std::string>(errorMessage);
    pm.startActivity(PageType::MESSAGE, msgParam);
}

// 示例：关闭消息页面并返回上一页
void closeMessagePage() {
    PageManager& pm = PageManager::getInstance();
    pm.finishActivity();
}