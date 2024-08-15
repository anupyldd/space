#pragma once

/*
 * .ltf (localization text file) is a custom file format suited for localization.
 * For a sample demonstrating the format, check the misc folder.
 * Make sure .ltf files you use are in UTF-8 encoding.
 *
 * It is recommended you use the LtfFile class to work with .ltf files.
 * Before parsing the contents use the Prepare() function, it will open 
 * the file and map its contents.
 * The class offers several ways to parse and then retrieve info from files.
 * 
 * You can create an index of a file, which will create a map with position offsets
 * for each ids and corresponding languages. Together with file memory mapping, this 
 * option offers good performance with smaller memory usage (since you don't need to
 * store text itself, it is retrieved at runtime). You can create index either for
 * a specific language (CreateIndex), or for the whole file (CreateIndexAll). 
 * You can have 1 index per file. Works well with large files.
 * 
 * Another option is to parse the file into a simple map where all the ids and 
 * translations are stored. You can either a map for a specific language (CreateMap),
 * or a map for the whole file (CreateMapAll). This works well with smaller localization 
 * files, since it uses more memory. You can have 1 map per file. 
 * 
 * It's recommended to select one of the options depending on the size of the file
 * (idk why you would want to create both index and map, but whatever floats ig).
 * Note that editing a file invalidates an index / map, and you'll have to re-generate them.
 */

#include "Fsm.h"
#include "Language.h"
#include "FileHandling.h"
#include "StringUtil.h"

#include <optional>
#include <string>
#include <sstream>
#include <vector>
#include <sstream>
#include <functional>
#include <filesystem>

namespace file
{
    using namespace lang;

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
            Language                    lan = Language::NONE;
            std::optional<std::string>  var = std::nullopt;

            friend bool operator==(const LanguageVariation& lhs, const LanguageVariation& rhs)
            {
                return lhs.lan == rhs.lan && lhs.var == rhs.var;
            }
        };

        struct LanVarHash
        {
            size_t operator()(const LanguageVariation& lanVar) const
            {
                return ((std::hash<Language>()(lanVar.lan)
                    ^ (std::hash<std::optional<std::string>>()(lanVar.var) << 1)) >> 1);
            }
        };

        class MultiStr
        {
        public:
            // for default variation
            MultiStr(Language ln, const std::string& str) { m_locMap[{ln}] = str; }
            void Set(Language ln, const std::string& str) { m_locMap[{ln}] = str; }
            std::string Get(Language ln) { return m_locMap.at({ ln }); }

            // for custom variation
            MultiStr(Language ln, const std::string& var, const String& str) { m_locMap[{ln, var}] = str; }
            void Set(Language ln, const std::string& var, const String& str) { m_locMap[{ln, var}] = str; }
            std::string Get(Language ln, const std::string& var) { return m_locMap.at({ ln, var }); }

        private:
            std::unordered_map<LanguageVariation, String, LanVarHash> m_locMap;
        };

        using IndexPair = std::pair<uint32_t, uint32_t>;

        class MultiLocIndex
        {
        public:
            // for default variation
            MultiLocIndex(Language ln, uint32_t begin, uint32_t end) { m_indexMap[{ln}] = { begin,end }; }
            void Set(Language ln, uint32_t begin, uint32_t end) { m_indexMap[{ln}] = { begin,end }; }
            IndexPair Get(Language ln) { return m_indexMap.at({ ln }); }

            // for custom variation
            MultiLocIndex(Language ln, const std::string& var, uint32_t begin, uint32_t end) { m_indexMap[{ln, var}] = { begin,end }; }
            void Set(Language ln, const std::string& var, uint32_t begin, uint32_t end) { m_indexMap[{ln, var}] = { begin,end }; }
            IndexPair Get(Language ln, const std::string& var) { return m_indexMap.at({ ln, var }); }

        private:
            std::unordered_map<LanguageVariation, IndexPair, LanVarHash> m_indexMap;
        };
    }

    class LtfFile : public File
    {
    public:
        bool Prepare(const std::filesystem::path& path)
        {
            Open(path, FileMode::READ);
            Map();
        }
        
        bool CreateIndex(Language ln)
        {
            if (!m_ready) return false;
            if (m_locIndex) m_locIndex.value().clear();

        }

        bool CreateIndexAll()
        {
            if (!m_ready) return false;
            if (m_locIndex) m_locIndex.value().clear();

        }

        bool CreateMap(Language ln)
        {
            if (!m_ready) return false;
            if (m_locMap) m_locMap.value().clear();

        }

        bool CreateMapAll()
        {
            if (!m_ready) return false;
            if (m_locMap) m_locMap.value().clear();

        }

    private:
        void m_Parse(std::function<void()> saveFunc)
        {
            std::istringstream isstr(GetContent());
            for (std::string line; std::getline(isstr, line);)
            {
                if (line.starts_with("//")) continue;   // skip one line comments
            }
        }

    private:
        bool                                                                m_ready = false;
        std::optional<std::unordered_map<std::string, itrn::MultiStr>>      m_locMap;
        std::optional<std::unordered_map<std::string, itrn::MultiLocIndex>> m_locIndex;
    };
    
}