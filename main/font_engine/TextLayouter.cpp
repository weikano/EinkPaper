#include "TextLayouter.h"

#include "Utf8Decoder.h"

#include <algorithm>

namespace font_engine {

TextLayout TextLayouter::layoutUtf8(const std::string& utf8,
                                    const FontMetrics& metrics,
                                    int16_t maxWidth,
                                    const AdvanceResolver& advanceResolver,
                                    int16_t lineSpacing) const {
    TextLayout result;
    result.lineHeight = metrics.lineHeight;
    result.baseline = metrics.baseline;

    if (metrics.glyphWidth == 0 || metrics.glyphHeight == 0) {
        return result;
    }

    if (maxWidth <= 0) {
        maxWidth = metrics.glyphWidth;
    }

    int16_t cursorX = 0;
    int16_t cursorY = 0;
    int16_t maxLineWidth = 0;
    uint16_t lines = 1;

    for (size_t i = 0; i < utf8.size();) {
        const char current = utf8[i];
        if (current == '\n') {
            maxLineWidth = std::max(maxLineWidth, cursorX);
            cursorX = 0;
            cursorY += static_cast<int16_t>(metrics.lineHeight + lineSpacing);
            ++lines;
            ++i;
            continue;
        }

        Utf8Decoder::Result decoded = Utf8Decoder::decodeNext(&utf8[i], utf8.size() - i);
        if (decoded.consumed == 0) {
            break;
        }
        i += decoded.consumed;
        int16_t advanceX = static_cast<int16_t>(metrics.glyphWidth);
        if (advanceResolver) {
            advanceX = advanceResolver(decoded.codepoint);
        }
        if (advanceX <= 0) {
            advanceX = static_cast<int16_t>(metrics.glyphWidth);
        }

        if (cursorX > 0 && cursorX + advanceX > maxWidth) {
            maxLineWidth = std::max(maxLineWidth, cursorX);
            cursorX = 0;
            cursorY += static_cast<int16_t>(metrics.lineHeight + lineSpacing);
            ++lines;
        }

        result.glyphs.push_back({decoded.codepoint, cursorX, cursorY});
        cursorX += advanceX;
    }

    maxLineWidth = std::max(maxLineWidth, cursorX);

    result.width = maxLineWidth;
    result.lineCount = lines;
    result.height = static_cast<int16_t>(lines * metrics.lineHeight + (lines - 1) * lineSpacing);
    return result;
}

}  // namespace font_engine
