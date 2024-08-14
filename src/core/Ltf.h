#pragma once

// .ltf (localization text file) is a custom file format suited for localization
// for a sample demonstrating the format check the misc folder.

#include "Fsm.h"
#include "Language.h"

#include <optional>
#include <string>
#include <vector>
#include <sstream>

namespace ltf
{
    using namespace lang;

    class Parser
    {
    public:
        void Init()
        {
            LoadLanguageMap(m_langCodes);
        }

        bool CreateIndex()
        {

        }

    private:
        std::unordered_map<std::string, Language> m_langCodes;

    private:
        constexpr char O_BRACKET = '[';
        constexpr char C_BRACKET = ']';
        constexpr char O_BRACE = '{';
        constexpr char O_BRACE = '}';
        constexpr char DOT = '.';
        constexpr char F_SLASH = '/';
        constexpr char B_SLASH = '\\';
        constexpr char STAR = '*';

        enum class TokenType { ID, LANG, TEXT, INSERT_ID, INSERT_ARG, END_OF_FILE };

        struct Token
        {
            TokenType type;
            std::string value;
            std::optional<std::string> langVar;
        };
    };
}