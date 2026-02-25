#pragma once

#include "../config/DeviceConfig.h"

#include <array>
#include <cstdint>

enum class StringId : uint16_t {
#define X(ID, ZH, EN, ZH_TW) ID,
#include "I18nStrings.def"
#undef X
    COUNT
};

class I18nManager {
public:
    static I18nManager& getInstance();

    void init(Language::LanguageEnum language);
    void setLanguage(Language::LanguageEnum language);
    Language::LanguageEnum getLanguage() const { return _language; }

    const char* tr(StringId id) const;

private:
    I18nManager() = default;

    Language::LanguageEnum _language = Language::Chinese;
};

inline const char* tr(StringId id) {
    return I18nManager::getInstance().tr(id);
}
