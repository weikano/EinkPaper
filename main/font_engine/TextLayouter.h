#pragma once

#include "FontTypes.h"

#include <cstdint>
#include <functional>
#include <string>

namespace font_engine {

class TextLayouter {
public:
    using AdvanceResolver = std::function<int16_t(uint32_t codepoint)>;

    TextLayout layoutUtf8(const std::string& utf8,
                          const FontMetrics& metrics,
                          int16_t maxWidth,
                          const AdvanceResolver& advanceResolver,
                          int16_t lineSpacing = 0) const;
};

}  // namespace font_engine
