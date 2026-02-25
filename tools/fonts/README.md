## 使用指南
### 一 下载oft字体文件
> 从 [Adobe Source Han Serif 官方仓库的 Releases](https://github.com/adobe-fonts/source-han-serif/releases) 中，下载 OTF 版本，你需要下面 4 个 Regular 字重文件，即下载链接中的`Language Specific OTFs`：
- 简体中文 SourceHanSerifSC-Regular.otf
- 繁体中文 SourceHanSerifTC-Regular.otf
- 日文 SourceHanSerifJP-Regular.otf
- 韩文 SourceHanSerifKR-Regular.otf
### 二 将字体文件放到`tools/fonts/SourceHanSerif`目录下
> 注意压缩包中日文的字体otf文件是`SourceHanSerif.otf`

解压后将所有文件复制到`tools/fonts/SourceHanSerif`目录下
### 生成`font`文件
> 进入`tools/fonts`目录，执行下面命令：
```bash
python3 build_font.py <font_px> <style>
``` 
其中`font_px`是字体的像素大小，`style`是字体样式，例如：
```bash
python3 build_font.py 16 Regular
```
执行后会在`tools/fonts`目录下生成对应的`font`文件
