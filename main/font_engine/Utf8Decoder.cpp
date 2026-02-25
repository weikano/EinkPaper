#include "Utf8Decoder.h"

namespace font_engine {

Utf8Decoder::Result Utf8Decoder::decodeNext(const char* bytes, size_t length) {
    Result result;
    if (!bytes || length == 0) {
        return result;
    }

    const uint8_t b0 = static_cast<uint8_t>(bytes[0]);

    if ((b0 & 0x80u) == 0) {
        result.codepoint = b0;
        result.consumed = 1;
        result.valid = true;
        return result;
    }

    if ((b0 & 0xE0u) == 0xC0u && length >= 2) {
        const uint8_t b1 = static_cast<uint8_t>(bytes[1]);
        if ((b1 & 0xC0u) == 0x80u) {
            result.codepoint = ((b0 & 0x1Fu) << 6) | (b1 & 0x3Fu);
            result.consumed = 2;
            result.valid = result.codepoint >= 0x80;
            return result;
        }
    }

    if ((b0 & 0xF0u) == 0xE0u && length >= 3) {
        const uint8_t b1 = static_cast<uint8_t>(bytes[1]);
        const uint8_t b2 = static_cast<uint8_t>(bytes[2]);
        if ((b1 & 0xC0u) == 0x80u && (b2 & 0xC0u) == 0x80u) {
            result.codepoint = ((b0 & 0x0Fu) << 12) | ((b1 & 0x3Fu) << 6) | (b2 & 0x3Fu);
            result.consumed = 3;
            result.valid = result.codepoint >= 0x800;
            return result;
        }
    }

    if ((b0 & 0xF8u) == 0xF0u && length >= 4) {
        const uint8_t b1 = static_cast<uint8_t>(bytes[1]);
        const uint8_t b2 = static_cast<uint8_t>(bytes[2]);
        const uint8_t b3 = static_cast<uint8_t>(bytes[3]);
        if ((b1 & 0xC0u) == 0x80u && (b2 & 0xC0u) == 0x80u && (b3 & 0xC0u) == 0x80u) {
            result.codepoint = ((b0 & 0x07u) << 18) | ((b1 & 0x3Fu) << 12) |
                               ((b2 & 0x3Fu) << 6) | (b3 & 0x3Fu);
            result.consumed = 4;
            result.valid = result.codepoint >= 0x10000 && result.codepoint <= 0x10FFFF;
            return result;
        }
    }

    result.codepoint = 0xFFFD;
    result.consumed = 1;
    result.valid = false;
    return result;
}

}  // namespace font_engine
