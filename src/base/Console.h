#include <cstdint>
#include <type_traits>
#include <format>

// API ---------------------------------

namespace spc
{
    namespace con
    {
        inline void Init(); // must be called before any printing

        inline std::wstring Utf8ToUtf16(const String& str8);
        inline String Utf16ToUtf8(const std::wstring& str16);

        inline void Print(const String& str);
        inline void PrintN(const String& str);
        inline void Input(String& str);

        enum ColorANSI : uint8_t
        {
            NONE            = 0,
            BLACK           = 30,
            RED             = 31,
            GREEN           = 32,
            YELLOW          = 33,
            BLUE            = 34,
            MAGENTA         = 35,
            CYAN            = 36,
            WHITE           = 37,
            BRIGHT_BLACK    = 90,
            BRIGHT_RED      = 91,
            BRIGHT_GREEN    = 92,
            BRIGHT_YELLOW   = 93,
            BRIGHT_BLUE     = 94,
            BRIGHT_MAGENTA  = 95,
            BRIGHT_CYAN     = 96,
            BRIGHT_WHITE    = 97
        };
    }
}

// -------------------------------------

namespace spc
{

#if defined(_WIN64) || defined(_WIN32)
    // Windows ----------

    #include "WinDef.h"

    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif

    static HANDLE stdoutHandle;
    static DWORD outModeInit;

    namespace con
    {
        // internal functions
        namespace itrn
        {
            inline void EnableANSI()
        {
            DWORD outMode = 0;
            stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

            if (stdoutHandle == INVALID_HANDLE_VALUE)
            {
                exit(GetLastError());
            }

            if (!GetConsoleMode(stdoutHandle, &outMode))
            {
                exit(GetLastError());
            }

            outModeInit = outMode;
            outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;

            if (!SetConsoleMode(stdoutHandle, outMode))
            {
                exit(GetLastError());
            }
        }

            // for getting underlying value from enum class members
            template<typename T>
            constexpr inline auto GetUnderlying(T ecm) -> typename std::underlying_type<T>::type
            {
                return static_cast<typename std::underlying_type<T>::type>(ecm);
            }
        }
        // -----------------
        inline void Init()
        {
            itrn::EnableANSI();
            system("chcp 65001");
        }

        inline std::wstring Utf8ToUtf16(const String& str8) 
        {
            int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str8.c_str(), (int)str8.size(), NULL, 0);
            std::wstring str16(sizeNeeded, 0);
            MultiByteToWideChar(CP_UTF8, 0, str8.c_str(), (int)str8.size(), &str16[0], sizeNeeded);
            return str16;
        }

        inline String Utf16ToUtf8(const std::wstring& str16) 
        {
            int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str16.c_str(), (int)str16.size(), NULL, 0, NULL, NULL);
            String str8(sizeNeeded, 0);
            WideCharToMultiByte(CP_UTF8, 0, str16.c_str(), (int)str16.size(), &str8[0], sizeNeeded, NULL, NULL);
            return str8;
        }

        inline void Print(const String& str) 
        {
            std::wcout << Utf8ToUtf16(str);
        }

        inline void PrintN(const String& str) 
        {
            std::wcout << Utf8ToUtf16(str) << L'\n';
        }

        inline void Input(String& str) 
        {
            std::wstring wstr;
            std::wcin >> wstr;
            str = Utf16ToUtf8(wstr);
        }

        inline String SetStringColor(const String& str, ColorANSI txtCol, ColorANSI bgCol = ColorANSI::NONE)
        {
            if (bgCol == ColorANSI::NONE)
                return std::format("\x1b[{}m{}\x1b[0m", itrn::GetUnderlying(txtCol), str);

            return std::format("\x1b[{};{}m{}\x1b[0m",
                itrn::GetUnderlying(txtCol), itrn::GetUnderlying(bgCol) + 10, str);
        }
    }

#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
    // Unix -------------

    namespace con
    {
        inline void Init() {}

        inline void Print(const String& str) { std::cout << str; }
        inline void PrintN(const String& str) { std::cout << str << '\n'; }
        inline void Input(String& str) { std::cin >> str; }
    }

#else

#error "Platform not supported"

#endif

}
