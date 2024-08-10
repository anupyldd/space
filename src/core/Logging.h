#pragma once

// use this for logging, not console directly

#include "Types.h"
#include "Console.h"

namespace spc
{
    namespace log
    {
        inline void Output(const String& str)
        {
            con::Print(str);
        }

        inline void Debug(const String& str)
        {
            #if _DEBUG
            con::PrintN(con::SetStringColor("[DEBUG]   " + str, con::BLUE, con::NONE));
            #endif
        }

        inline void Info(const String& str)
        {
            con::PrintN(con::SetStringColor("[INFO]    " + str, con::GREEN, con::NONE));
        }

        inline void Warning(const String& str)
        {
            con::PrintN(con::SetStringColor("[WARNING] " + str, con::BLACK, con::YELLOW));
        }

        inline void Error(const String& str)
        {
            con::PrintN(con::SetStringColor("[ERROR]   " + str, con::WHITE, con::RED));
        }
    }
}