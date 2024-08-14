#pragma once

#include "../core/Core.h"
#include "../core/Language.h"

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
        using namespace lang;

        class MultiStr; // can store multiple variations of a string for different languages

        // -------------------------

        class Localization
        {
        public:
            void            LoadFiles(std::initializer_list<std::filesystem::path> paths);  // files will be added to a map, where keys are file names and values are LocFile objects
            void            UnloadFiles(std::initializer_list<String> fileNames);
            void            UnloadFilesAll();

            void            CreateFileIndex(std::initializer_list<String> fileNames);  // parses requested files and creates index for tags to make searching quicker
            void            LoadFileIntoMap(const String& fileName, std::unordered_map<String, String>& map);   // loads a file into a provided tag-text map
            
            String          GetStrByTag(const String& tag) const;
            String          GetFileContents(const String& fileName);

            uint16_t        GetLoadedFilesNum() const;

            void            SetLanguage(Language lang);
            static Language GetLanguage();

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
            static Language                     m_gameLang;
            std::unordered_map<String, LocFile> m_loadedLocFiles;
        };

        // -------------------------

        class MultiStr
        {
        public:
            MultiStr() = default;
            MultiStr(Language lang, const String& str) { m_locMap[lang] = str; }

            void Set(Language lang, const String& str) { m_locMap[lang] = str; }

            String operator()() { return m_locMap.at(Localization::GetLanguage()); }
            String operator()(Language lang) { return m_locMap.at(lang); }

        private:
            std::unordered_map<Language, String> m_locMap;
        };
    }
}