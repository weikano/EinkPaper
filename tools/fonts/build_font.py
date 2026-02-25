#!/usr/bin/env python3
import sys
import freetype
import struct
import numpy as np
from fontTools.ttLib import TTFont
from pathlib import Path
import shutil

# ==================================================
# 参数解析
# ==================================================
if len(sys.argv) != 3:
    print("Usage: python build_cjk_font.py <font_px> <style>")
    print("Example: python build_cjk_font.py 24 Regular")
    sys.exit(1)

FONT_SIZE = int(sys.argv[1])
STYLE = sys.argv[2]

GLYPH_W = FONT_SIZE
GLYPH_H = FONT_SIZE

# ==================================================
# 字体配置
# ==================================================
FONT_FILES = [
    f"SourceHanSerif/SourceHanSerifTW-{STYLE}.otf", # 繁体中文
    f"SourceHanSerif/SourceHanSerifSC-{STYLE}.otf", # 简体中文
    f"SourceHanSerif/SourceHanSerif-{STYLE}.otf", # 日文
    f"SourceHanSerif/SourceHanSerifK-{STYLE}.otf", # 韩文
]

for f in FONT_FILES:
    if not Path(f).exists():
        raise FileNotFoundError(f"Missing font file: {f}")

OUTPUT_FILE = f"cjk_{FONT_SIZE}px_{STYLE}_1bpp.font"
ASSETS_FONT_DIR = Path(__file__).resolve().parents[2] / "main" / "assets" / "fonts"
INDEX_ENTRY_SIZE = 16  # u32 codepoint + u32 offset + i16 adv + i16 bearingX + i16 bearingY + u16 reserved

UNICODE_RANGES = [
    # ASCII
    (0x0020, 0x007E),

    # Latin-1 Supplement（常见符号）
    (0x00A0, 0x00FF),

    # General Punctuation
    (0x2000, 0x206F),

    # Currency Symbols
    (0x20A0, 0x20CF),

    # CJK Symbols and Punctuation（‼️繁中核心）
    (0x3000, 0x303F),
    (0xFE10, 0xFE1F),   # Vertical forms
    (0xFE30, 0xFE4F),   # CJK compatibility forms

    # Hiragana / Katakana
    (0x3040, 0x309F),
    (0x30A0, 0x30FF),
    (0x31F0, 0x31FF),   # Katakana phonetic extensions

    # Bopomofo / CJK radicals & strokes
    (0x3100, 0x312F),   # Bopomofo
    (0x31A0, 0x31BF),   # Bopomofo Extended
    (0x2E80, 0x2EFF),   # CJK Radicals Supplement
    (0x31C0, 0x31EF),   # CJK Strokes

    # Fullwidth and compatibility area
    (0xFF00, 0xFFEF),
    (0xF900, 0xFAFF),   # CJK Compatibility Ideographs

    # CJK Unified Ideographs
    (0x4E00, 0x9FFF),

    # CJK Extension A
    (0x3400, 0x4DBF),

    # CJK Extension B（可选但强烈建议）
    (0x20000, 0x2A6DF),

    # Hangul
    (0xAC00, 0xD7AF),
]

# ==================================================
# 初始化字体
# ==================================================
faces = []
cmaps = []

for font_path in FONT_FILES:
    face = freetype.Face(font_path)
    face.set_pixel_sizes(0, FONT_SIZE)
    faces.append(face)

    tt = TTFont(font_path)
    cmaps.append(tt["cmap"].getBestCmap())

# ==================================================
# Glyph 渲染
# ==================================================
def render_glyph(face, char):
    face.load_char(
        char,
        freetype.FT_LOAD_RENDER |
        freetype.FT_LOAD_TARGET_MONO |
        freetype.FT_LOAD_MONOCHROME
    )
    bmp = face.glyph.bitmap

    canvas = np.zeros((GLYPH_H, GLYPH_W), dtype=np.uint8)

    for y in range(bmp.rows):
        for x in range(bmp.width):
            byte = bmp.buffer[y * bmp.pitch + (x >> 3)]
            if byte & (0x80 >> (x & 7)):
                bx = face.glyph.bitmap_left + x
                by = GLYPH_H - (face.glyph.bitmap_top - y)
                if 0 <= bx < GLYPH_W and 0 <= by < GLYPH_H:
                    canvas[by][bx] = 1
    advance_x = int(round(face.glyph.advance.x / 64.0))
    if advance_x <= 0:
        advance_x = GLYPH_W
    bearing_x = int(face.glyph.bitmap_left)
    bearing_y = int(face.glyph.bitmap_top)
    return canvas, advance_x, bearing_x, bearing_y

def pack_1bpp(bitmap):
    out = bytearray()
    for y in range(GLYPH_H):
        byte = 0
        bits = 0
        for x in range(GLYPH_W):
            byte = (byte << 1) | bitmap[y][x]
            bits += 1
            if bits == 8:
                out.append(byte)
                byte = 0
                bits = 0
        if bits:
            out.append(byte << (8 - bits))
    return out

# ==================================================
# 收集 Glyph（多字体 fallback）
# ==================================================
glyph_map = {}
print(f"Building {FONT_SIZE}px {STYLE}...")

for start, end in UNICODE_RANGES:
    for code in range(start, end + 1):
        for face, cmap in zip(faces, cmaps):
            if code in cmap:
                try:
                    bmp, advance_x, bearing_x, bearing_y = render_glyph(face, chr(code))
                    data = pack_1bpp(bmp)
                    # 重叠区间或多字体 fallback 时，保留首次命中的字形
                    if code not in glyph_map:
                        glyph_map[code] = (data, advance_x, bearing_x, bearing_y)
                except:
                    pass
                break

glyphs = sorted(glyph_map.items(), key=lambda item: item[0])
print(f"Total glyphs: {len(glyphs)}")

# ==================================================
# 写入字库文件
# ==================================================
with open(OUTPUT_FILE, "wb") as f:
    f.write(b"CJK2")
    f.write(struct.pack("<I", len(glyphs)))
    f.write(struct.pack("<HHBB", GLYPH_W, GLYPH_H, 1, INDEX_ENTRY_SIZE))

    offset = 0
    for code, (data, advance_x, bearing_x, bearing_y) in glyphs:
        f.write(struct.pack("<IIhhhH", code, offset, advance_x, bearing_x, bearing_y, 0))
        offset += len(data)

    for _, (data, _, _, _) in glyphs:
        f.write(data)

print("Done:", OUTPUT_FILE)

# ==================================================
# 同步复制到 assets/fonts
# ==================================================
ASSETS_FONT_DIR.mkdir(parents=True, exist_ok=True)
assets_font_path = ASSETS_FONT_DIR / OUTPUT_FILE
shutil.copy2(OUTPUT_FILE, assets_font_path)
print("Copied to:", assets_font_path)
