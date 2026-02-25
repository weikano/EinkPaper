#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace font_engine {

class Utf8Decoder {
public:
    struct Result {
        uint32_t codepoint = 0;
        size_t consumed = 0;
        bool valid = false;
    };

    static Result decodeNext(const char* bytes, size_t length);
};

}  // namespace font_engine
