#pragma once

/*
 * .ltf (localization text file) is a custom file format suited for localization.
 * For a sample demonstrating the format, check the misc folder.
 * Make sure .ltf files you use are in UTF-8 encoding.
 *
 * It is recommended you use the LtfFile class to work with .ltf files.
 * The class offers several ways to parse and then retrieve info from files.
 * 
 * You can create an index of a file, which will create a map with position offsets
 * for each ids and corresponding languages. Together with file memory mapping, this 
 * option offers good performance with smaller memory usage (since you don't need to
 * store text itself, it is retrieved at runtime). Works well with large files.
 * 
 * Another option is to parse the file into a simple map where all the ids and 
 * translations are stored. You can either a map for a specific language (CreateMap),
 * or a map for the whole file (CreateMapAll). This works well with smaller localization 
 * files, since it uses more memory.
 * 
 * It's recommended to select one of the options depending on the size of the file
 * (idk why you would want to create both index and map, but whatever floats ig).
 * Note that editing a file invalidates an index / map, and you'll have to re-generate them.
 */

#include "Fsm.h"
#include "Language.h"
#include "FileHandling.h"

#include <optional>
#include <string>
#include <vector>
#include <sstream>

namespace file
{
    namespace itrn
    {
        inline constexpr char O_BRACKET = '[';
        inline constexpr char C_BRACKET = ']';
        inline constexpr char O_BRACE = '{';
        inline constexpr char C_BRACE = '}';
        inline constexpr char DOT = '.';
        inline constexpr char F_SLASH = '/';
        inline constexpr char B_SLASH = '\\';
        inline constexpr char STAR = '*';

        inline const std::string MISSING_ARG = "MISSING_ARGUMENT";
        inline const std::string MISSING_TRANSLATION = "MISSING_TRANSLATION";

        enum class TokenType { ID, LANG, TEXT, INSERT_ID, INSERT_ARG, END_OF_FILE };

        struct Token
        {
            TokenType type;
            std::string value;
            std::optional<std::string> langVar;
        };

        struct LanguageVariation
        {
            Language                    lang = Language::NONE;
            std::optional<std::string>  var = std::nullopt;
        };

        class MultiStr
        {
        public:
            MultiStr() = default;
            // for default variation
            MultiStr(Language lang, const std::string& str) { m_locMap[{lang}] = str; }
            void Set(Language lang, const std::string& str) { m_locMap[{lang}] = str; }
            std::string Get(Language lang) { return m_locMap.at({ lang }); }

            // for custom variation
            MultiStr(Language lang, const std::string& var, const String& str) { m_locMap[{lang, var}] = str; }
            void Set(Language lang, const std::string& var, const String& str) { m_locMap[{lang, var}] = str; }
            std::string Get(Language lang, const std::string& var) { return m_locMap.at({ lang, var }); }

        private:
            std::unordered_map<LanguageVariation, String> m_locMap;
        };

        using IndexPair = std::pair<uint32_t, uint32_t>;

        class MultiLocIndex
        {
        public:
            // for default variation
            MultiLocIndex(Language lang, uint32_t begin, uint32_t end) { m_indexMap[{lang}] = { begin,end }; }
            void Set(Language lang, uint32_t begin, uint32_t end) { m_indexMap[{lang}] = { begin,end }; }
            IndexPair Get(Language lang) { return m_indexMap.at({ lang }); }

            // for custom variation
            MultiLocIndex(Language lang, const std::string& var, uint32_t begin, uint32_t end) { m_indexMap[{lang, var}] = { begin,end }; }
            void Set(Language lang, const std::string& var, uint32_t begin, uint32_t end) { m_indexMap[{lang, var}] = { begin,end }; }
            IndexPair Get(Language lang, const std::string& var) { return m_indexMap.at({ lang, var }); }

        private:
            std::unordered_map<LanguageVariation, IndexPair> m_indexMap;
        };
    }

    class LtfFile : public File
    {
    public:
        bool CreateIndex()
        {

        }

        bool CreateMap(Language lang)
        {

        }

        bool CreateMapAll()
        {

        }

    private:
        std::optional<std::unordered_map<std::string, itrn::MultiStr>>    m_locMap;
        
    };
    
}