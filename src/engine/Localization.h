#pragma once

#include "../core/Core.h"

#include <initializer_list>
#include <vector>
#include <filesystem>
#include <unordered_map>
#include <utility>
#include <format>
#include <cstdint>

namespace eng
{
    namespace loc
    {
        struct MultiStr;

        enum class Language;
        inline String GetLanguageCode(Language lang);


        // -------------------------

        class Localization
        {
        public:
            Localization() = default;

            void        LoadFiles(std::initializer_list<std::filesystem::path> paths);  // files will be added to a map, where keys are file names and values are LocFile objects
            void        UnloadFiles(std::initializer_list<String> fileNames);
            void        UnloadFilesAll();

            void        CreateFileIndex(std::initializer_list<String> fileNames);  // parses requested files and creates index for tags to make searching quicker
            void        LoadFileIntoMap(const String& fileName, std::unordered_map<String, String>& map);   // loads a file into a provided tag-text map
            String      GetStrByTag(const String& tag) const;
            String      GetFileContents(const String& fileName);

            uint16_t    GetLoadedFilesNum() const;

            void        SetLanguage(Language lang);
            Language    GetLanguage() const;

        private:
            struct LocFile
            {
                file::File  m_file;
                bool        m_indexed;
                std::unordered_map<String, uint32_t> m_tagIndex;
            };

        private:
            void m_ParseFile();


        private:
            Language                                m_gameLang;
            std::unordered_map<String, LocFile>  m_loadedLocFiles;
        };

        // -------------------------

        enum class Language
        {
            ENGLISH,
            RUSSIAN,
            JAPANESE
        };

        inline String GetLanguageCode(Language lang)
        {
            switch (lang)
            {
            case Language::ENGLISH: return "en";
            case Language::RUSSIAN: return "ru";
            case Language::JAPANESE: return "jp";
            default: throw exc::EngineException("Unknown language");
            }
        }
    }
}