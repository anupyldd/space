#pragma once

#include <unordered_map>
#include <string>
#include <optional>
#include <cstdint>
#include <utility>

namespace lang
{
    enum class Language
    {
        NONE,
        ENGLISH,
        RUSSIAN,
        JAPANESE,
        CHINESE,
        SPANISH,
        ARABIC,
        GERMAN,
        PORTUGESE,
        FRENCH,
        HINDI
    };

    inline std::unordered_map<std::string, Language> langCodesSL;  // initialize on Localization init

    inline String GetLanguageCodeStr(Language lang) noexcept
    {
        switch (lang)
        {
        case Language::ENGLISH:     return "en";
        case Language::RUSSIAN:     return "ru";
        case Language::JAPANESE:    return "ja";
        case Language::CHINESE:     return "zh";
        case Language::SPANISH:     return "es";
        case Language::ARABIC:      return "ar";
        case Language::GERMAN:      return "de";
        case Language::PORTUGESE:   return "pt";
        case Language::FRENCH:      return "fr";
        case Language::HINDI:       return "hi";
        default:                    return "none";
        }
    }

    inline Language GetLanguageCodeEnum(const std::string& code) noexcept
    {
        if (code == "en") return Language::ENGLISH;
        else if (code == "ru") return Language::RUSSIAN;
        else if (code == "ja") return Language::JAPANESE;
        else if (code == "zh") return Language::CHINESE;
        else if (code == "es") return Language::SPANISH;
        else if (code == "ar") return Language::ARABIC;
        else if (code == "de") return Language::GERMAN;
        else if (code == "pt") return Language::PORTUGESE;
        else if (code == "fr") return Language::FRENCH;
        else if (code == "hi") return Language::HINDI;
        else return Language::NONE;
    }

    inline void LoadLanguageMap(std::unordered_map<Language, std::string>& map)
    {
        map[Language::ENGLISH]      = "en";
        map[Language::RUSSIAN]      = "ru";
        map[Language::JAPANESE]     = "ja";
        map[Language::CHINESE]      = "zh";
        map[Language::SPANISH]      = "es";
        map[Language::ARABIC]       = "ar";
        map[Language::GERMAN]       = "de";
        map[Language::PORTUGESE]    = "pt";
        map[Language::FRENCH]       = "fr";
        map[Language::HINDI]        = "hi";
    }

    inline void LoadLanguageMap(std::unordered_map<std::string, Language>& map)
    {
        map["en"] = Language::ENGLISH;
        map["ru"] = Language::RUSSIAN;
        map["ja"] = Language::JAPANESE;
        map["zh"] = Language::CHINESE;
        map["es"] = Language::SPANISH;
        map["ar"] = Language::ARABIC;
        map["de"] = Language::GERMAN;
        map["pt"] = Language::PORTUGESE;
        map["fr"] = Language::FRENCH;
        map["hi"] = Language::HINDI;
    }
}