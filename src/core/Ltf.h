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

            auto& GerStrMap() { return m_locMap; }

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

            auto& GerIndexMap() { return m_indexMap; }

        private:
            std::unordered_map<LanguageVariation, IndexPair, LanVarHash> m_indexMap;
        };

        // for ltf parser
        struct LtfEntryBuilder;
        const std::regex ltfIdReg("^[A-Za-z0-9_-]+$");

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
            return false;
        }
        
        bool CreateIndex(Language ln)
        {
            if (!m_ready) return false;
            m_locIndex.clear();

        }

        bool CreateIndexAll()
        {
            if (!m_ready) return false;
            m_locIndex.clear();

        }

        bool CreateMap(Language ln)
        {
            if (!m_ready) return false;
            m_locMap.clear();
            m_Parse(ParseDest::MAP, ln);
        }

        bool CreateMapAll()
        {
            if (!m_ready) return false;
            m_locMap.clear();

        }

    public:

        std::unordered_map<std::string, itrn::MultiStr>& GetMap() { return m_locMap; }
        std::unordered_map<std::string, itrn::MultiLocIndex>& GetIndex() { return m_locIndex; }

    private:
        friend struct itrn::LtfEntryBuilder;
        enum class ParseDest { MAP, INDEX };

        bool m_Parse(ParseDest dest, Language ln = Language::NONE)
        {
            using namespace itrn;
            m_entryBuilder.Clear();
            m_curState = ltfGlobalState;

            try
            {
                std::string contents = GetContent();
                std::string_view sv(contents);
                for (size_t i = 0; i < sv.size(); i++)
                {
                    m_curState->Process(*this, m_entryBuilder, sv, i, dest, ln);
                    m_entryBuilder.SetValue(dest);
                    
                    lg::Debug(std::format("LtfEntryBuilder state:\n- begin: {}\n- end: {}\n- lan: {}\n- id: {}\n- var: {}\n- text: {}",
                        (void*)m_entryBuilder.begin, (void*)m_entryBuilder.end, lang::GetLanguageCodeStr(m_entryBuilder.lan), m_entryBuilder.id, m_entryBuilder.var, m_entryBuilder.text.str()));
                }
            }
            catch (const exc::IException& e)
            {
                lg::Error(e.What());
                return false;
            }
            return true;
        }

    private:

        struct LtfEntryBuilder
        {
            void Clear()
            {
                begin = end = 0;
                lan = Language::NONE;
                id.clear();
                var.clear();
                text.clear();

                mind.GerIndexMap().clear();
                mstr.GerStrMap().clear();
            }

            void SetValue(ParseDest dest)
            {
                if (id.empty() || text.str().empty()) return;
                if (dest == ParseDest::MAP)
                {
                    if (var.empty()) mstr.Set(lan, text.str());
                    else mstr.Set(lan, var, text.str());
                    
                    text.clear();
                }
                else
                {
                    if (begin != 0 && end != 0)
                    {
                        if (var.empty()) mind.Set(lan, begin, end);
                        else mind.Set(lan, var, begin, end);

                        text.clear(); 
                    }
                }
            }

            void RegisterValue(LtfFile& f, ParseDest dest)
            {
                using namespace itrn;

                if (dest == ParseDest::MAP) f.m_locMap[id] = mstr;
                else f.m_locIndex[id] = mind;

                Clear();
            }

            bool readyMind = false;
            bool readyMstr = false;

            uint32_t begin, end;
            Language lan;
            std::string id, var;
            std::stringstream text;
            
            itrn::MultiLocIndex mind;
            itrn::MultiStr mstr;
        };

        struct LtfParserState
        {
            virtual void Process(LtfFile& owner, LtfEntryBuilder& bld, std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) = 0;
        };

        struct LtfGlobalState : public LtfParserState
        {
            virtual void Process(LtfFile& owner, LtfEntryBuilder& bld, std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) override
            {
                switch (sv[i])
                {
                case '/':
                {
                    if (sv[i + 1] == '/') i = sv.substr(i).find_first_of('\n');       // one line comment
                    else if (sv[i + 1] == '*') owner.m_curState = owner.ltfCommentState;   // multi line comment
                    else throw exc::CoreException(std::format("LTF parsing error: unexpected '/' at position {}", i));
                    break;
                }
                case '[':
                {
                    std::string val(sv.substr(i + 1, sv.substr(i).find_first_of(']') - 1));
                    val = util::Trim(val);
                    if (val.find('.') != val.npos || lang::langCodesSL.contains(val)) owner.m_curState = owner.ltfLangState;
                    else owner.m_curState = owner.ltfIdState;
                    break;
                }
                case ' ':
                case '\r':
                case '\t':
                case '\n':
                    owner.m_curState = owner.ltfGlobalState;
                    break;

                default:
                    throw exc::CoreException(std::format("LTF parsing error: unexpected '{}' at position {}", sv[i], i));
                    break;
                }
            }
        };

        struct LtfIdState : public LtfParserState
        {
            virtual void Process(LtfFile& owner, LtfEntryBuilder& bld, std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) override
            {
                using namespace itrn;

                if (!bld.id.empty())
                {
                    // if there's id and text or indices, then we're ready to register
                    if (!bld.text.str().empty() || !(bld.begin == 0 && bld.end == 0))
                    {
                        bld.RegisterValue(owner, dest);
                        owner.m_curState = owner.ltfGlobalState;
                    }
                    // if there's id but nothing else, then the entry is incorrect
                    else throw exc::CoreException(std::format("LTF parsing error: expected language code after identifier [{}]", bld.id));

                    return;
                }

                size_t idEnd = sv.substr(i).find_first_of(']');
                std::string id(sv.substr(i, idEnd));
                if (!CorrectLtfId(id)) throw exc::CoreException(std::format("LTF parsing error: invalid id [{}]", id));
                else
                {
                    bld.id = id;
                    i += idEnd;
                    owner.m_curState = owner.ltfGlobalState;
                }
            }
        };

        struct LtfLangState : public LtfParserState
        {
            virtual void Process(LtfFile& owner, LtfEntryBuilder& bld, std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) override
            {
                size_t lanEnd = sv.substr(i).find_first_of(']');
                std::string lan(sv.substr(i, lanEnd));
                if (lan.empty() || !lang::langCodesSL.contains(lan))
                    throw exc::CoreException(std::format("LTF parsing error: invalid language code: {}", lan));

                std::string code, var;
                if (lan.find('.') != lan.npos)
                {
                    size_t dot = lan.find_first_of('.');
                    code = lan.substr(0, dot);
                    var = lan.substr(dot + 1);

                    if (code.empty() || !lang::langCodesSL.contains(code))
                        throw exc::CoreException(std::format("LTF parsing error: invalid language code: {}", lan));
                    if (var.empty()) throw exc::CoreException(std::format("LTF parsing error: invalid language code: {}", lan));

                    bld.lan = lang::GetLanguageCodeEnum(code);
                    bld.var = var;
                }

                i += lanEnd;
                bld.lan = lang::GetLanguageCodeEnum(lan);

                owner.m_curState = owner.ltfTextState;
            }
        };

        struct LtfTextState : public LtfParserState
        {
            virtual void Process(LtfFile& owner, LtfEntryBuilder& bld, std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) override
            {

                if (sv[i] == '\n' && sv[i - 1] != '\\')
                {
                    owner.m_curState = owner.ltfGlobalState;
                    return;
                }

                bld.text << sv[i];
                owner.m_curState = owner.ltfTextState;
            }
        };

        struct LtfCommentState : public LtfParserState
        {
            virtual void Process(LtfFile& owner, LtfEntryBuilder& bld, std::string_view sv, size_t& i, ParseDest dest, Language ln = Language::NONE) override
            {
                for (size_t j = i; j < (sv.size() - i); ++j)
                {
                    if (sv[j] == '/' && sv[j - 1] == '*')
                    {
                        i = j;
                        break;
                    }
                }

                owner.m_curState = owner.ltfGlobalState;
            }
        };
        

    private:
        std::shared_ptr<LtfGlobalState> ltfGlobalState = std::make_shared<LtfGlobalState>();
        std::shared_ptr<LtfCommentState> ltfCommentState = std::make_shared<LtfCommentState>();
        std::shared_ptr<LtfTextState> ltfTextState = std::make_shared<LtfTextState>();
        std::shared_ptr<LtfLangState> ltfLangState = std::make_shared<LtfLangState>();
        std::shared_ptr<LtfIdState> ltfIdState = std::make_shared<LtfIdState>();

    private:
        bool                              m_ready = false;

        LtfEntryBuilder                   m_entryBuilder;
        std::shared_ptr<LtfParserState>   m_curState;

        std::unordered_map<std::string, itrn::MultiStr>      m_locMap;
        std::unordered_map<std::string, itrn::MultiLocIndex> m_locIndex;
    };
    
}