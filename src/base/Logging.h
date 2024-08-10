
#include "Types.h"
#include "Console.h"

namespace spc
{
    namespace log
    {
        inline void Debug(const String& str)
        {
            #if _DEBUG
            con::PrintN("[DEBUG] " + str);
            #endif
        }

        inline void Info(const String& str)
        {
            con::PrintN("[INFO] " + str);
        }

        inline void Warning(const String& str)
        {
            con::PrintN("[WARNING] " + str);
        }
    }
}