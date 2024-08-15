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
#include <variant>
#include <filesystem>
#include <regex>

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
            MultiStr() = default;
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
            MultiLocIndex() = default;
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

        const std::regex ltfIdReg("^[A-Za-z0-9]+$");

        bool CorrectLtfId(const std::string& id)
        {
            if (id.empty()) return false;
            if (std::isdigit(id[0])) return false;
            if (langCodesSL.contains(id)) return false;
            
            std::smatch match;
            std::regex_match(id, match, ltfIdReg);
            if (match.size() != 1) return false;

            return true;
        }

        // for ltf parser
        enum class ParseDest { MAP, INDEX };

        struct LtfParserState 
        { 
            virtual std::shared_ptr<LtfParserState> Process(std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) = 0;
        };

        struct LtfGlobalState : public LtfParserState
        {
            virtual std::shared_ptr<LtfParserState> Process(std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) override
            {
                switch (sv[i])
                {
                case '/':
                {
                    if (sv[i + 1] == F_SLASH) i = sv.find_first_of('\n');       // one line comment
                    else if (sv[i + 1] == STAR) return ltfCommentState;   // multi line comment
                    else throw exc::CoreException(std::format("LTF parsing error: unexpected '/' at position {}", i));
                }
                case '[':
                {
                    std::string val(sv.substr(i, sv.find_first_of(']')));
                    if (lang::langCodesSL.contains(val)) return ltfLangState;
                    else return ltfIdState;
                }
                case ' ':
                case '\t':
                    return ltfGlobalState;

                default:
                    throw exc::CoreException(std::format("LTF parsing error: unexpected '{}' at position {}", sv[i], i));
                }
            }
        };
        auto ltfGlobalState = std::make_shared<LtfGlobalState>();

        struct LtfIdState : public LtfParserState
        {
            virtual std::shared_ptr<LtfParserState> Process(std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) override
            {
                switch (sv[i])
                {


                default:
                    break;
                }
                return ltfGlobalState;
            }
        };
        auto ltfIdState = std::make_shared<LtfIdState>();

        struct LtfLangState : public LtfParserState
        {
            virtual std::shared_ptr<LtfParserState> Process(std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) override
            {
                return ltfGlobalState;
            }
        };
        auto ltfLangState = std::make_shared<LtfLangState>();

        struct LtfTextState : public LtfParserState
        {
            virtual std::shared_ptr<LtfParserState> Process(std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) override
            {
                return ltfGlobalState;
            }
        };
        auto ltfTextState = std::make_shared<LtfTextState>();

        struct LtfCommentState : public LtfParserState
        {
            virtual std::shared_ptr<LtfParserState> Process(std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) override
            {
                return ltfGlobalState;
            }
        };
        auto ltfCommentState = std::make_shared<LtfCommentState>();

    }

    class LtfFile : public File
    {
    public:
        bool Prepare(const std::filesystem::path& path)
        {
            if (Open(path, FileMode::READ))
            {
                if (Map()) m_ready = true;
            }
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
        enum class ParseTo : uint8_t { INDEX, MAP };
        enum class ParserState : uint8_t { GLOBAL, ID, LANG, TEXT, COMMENT };

        

        void m_Parse(ParseTo dest, Language ln = Language::NONE)
        {
            using namespace itrn;

            std::string id, var, text;
            Language ln;

            ParserState state = ParserState::GLOBAL;
            std::string_view sv(GetContent());
            for (size_t i = 0; i < sv.size(); ++i)
            {
                switch (state)
                {
                case ParserState::GLOBAL:
                {
                    if (sv[i] == F_SLASH)   // if comment
                    {
                        
                    }
                    else if (sv[i] == O_BRACKET)
                    {
                        
                    }
                    else throw exc::CoreException(std::format("LTF parsing error: unexpected '{}' at position {}", sv[i], i));
                    
                    break;
                }                    
                case ParserState::ID:
                {

                    id = sv.substr(i, sv.find_first_of(C_BRACKET));
                    if(!CorrectLtfId(id)) 
                        throw exc::CoreException(std::format("LTF parsing error: invalid id: {}", id));
                    else
                    {
                        
                    }
                    break;
                }
                case ParserState::LANG:
                {
                    break;
                }
                case ParserState::TEXT:
                {
                    break;
                }
                case ParserState::COMMENT:
                {
                    break;
                }
                default:
                    break;
                }
            }
        }

    private:
        bool                    m_ready = false;
        itrn::LtfParserState*   m_parseState = nullptr;
        std::optional<std::unordered_map<std::string, itrn::MultiStr>>      m_locMap;
        std::optional<std::unordered_map<std::string, itrn::MultiLocIndex>> m_locIndex;
    };
    
}