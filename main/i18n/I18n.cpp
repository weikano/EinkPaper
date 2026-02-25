#include "I18n.h"

#include <array>
#include <string_view>

namespace {
constexpr size_t kStringCount = static_cast<size_t>(StringId::COUNT);

constexpr std::array<std::string_view, kStringCount> kZhStrings = {
#define X(ID, ZH, EN, ZH_TW) ZH,
#include "I18nStrings.def"
#undef X
};

constexpr std::array<std::string_view, kStringCount> kEnStrings = {
#define X(ID, ZH, EN, ZH_TW) EN,
#include "I18nStrings.def"
#undef X
};

constexpr std::array<std::string_view, kStringCount> kZhTwStrings = {
#define X(ID, ZH, EN, ZH_TW) ZH_TW,
#include "I18nStrings.def"
#undef X
};

template <size_t N>
constexpr bool allNonEmpty(const std::array<std::string_view, N>& table) {
    for (const auto& text : table) {
        if (text.empty()) {
            return false;
        }
    }
    return true;
}

static_assert(allNonEmpty(kZhStrings), "zh_CN i18n table contains empty text");
static_assert(allNonEmpty(kEnStrings), "en_US i18n table contains empty text");
static_assert(allNonEmpty(kZhTwStrings), "zh_TW i18n table contains empty text");

const std::array<std::string_view, kStringCount>& getTable(Language::LanguageEnum language) {
    switch (language) {
        case Language::English:
            return kEnStrings;
        case Language::TraditionalChinese:
            return kZhTwStrings;
        case Language::Chinese:
        default:
            return kZhStrings;
    }
}
}  // namespace

I18nManager& I18nManager::getInstance() {
    static I18nManager instance;
    return instance;
}

void I18nManager::init(Language::LanguageEnum language) {
    setLanguage(language);
}

void I18nManager::setLanguage(Language::LanguageEnum language) {
    _language = language;
}

const char* I18nManager::tr(StringId id) const {
    const auto& table = getTable(_language);
    const auto index = static_cast<size_t>(id);
    if (index >= table.size()) {
        return "";
    }
    return table[index].data();
}
