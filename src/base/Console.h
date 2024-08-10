#pragma once

#include <cstdint>
#include <type_traits>
#include <format>
#include <locale>
#include <locale.h>
#include <codecvt>

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

        inline String SetStringColor(const String& str, ColorANSI txtCol, ColorANSI bgCol);
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

    #ifndef MS_STDLIB_BUGS
        #if ( _MSC_VER || __MINGW32__ || __MSVCRT__ )
            #define MS_STDLIB_BUGS 1
        #else
            #define MS_STDLIB_BUGS 0
        #endif
    #endif

    #if MS_STDLIB_BUGS
        #include <io.h>
        #include <fcntl.h>
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

            inline void InitLocale()
            {
            #if MS_STDLIB_BUGS
                constexpr char cp_utf16le[] = ".1200";
                setlocale(LC_ALL, cp_utf16le);
                _setmode(_fileno(stdout), _O_WTEXT);
            #else
                // The correct locale name may vary by OS, e.g., "en_US.utf8".
                constexpr char locale_name[] = "";
                setlocale(LC_ALL, locale_name);
                std::locale::global(std::locale(locale_name));
                std::wcin.imbue(std::locale())
                    std::wcout.imbue(std::locale());
            #endif
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
            itrn::InitLocale();
            itrn::EnableANSI();
            SetConsoleOutputCP(65001);
            //system("chcp 65001");
        }

        inline std::wstring Utf8ToUtf16(const String& str)
        {
            if (str.empty()) return std::wstring();

            int required = MultiByteToWideChar(CP_THREAD_ACP, 0, str.data(), (int)str.size(), NULL, 0);
            if (required == 0) return std::wstring();

            std::wstring wstr;
            wstr.resize(required);

            int converted = MultiByteToWideChar(CP_THREAD_ACP, 0, str.data(), (int)str.size(), &wstr[0], wstr.capacity());
            if (converted == 0) return std::wstring();

            return wstr;
        }

        inline std::string Utf16ToUtf8(const std::wstring& wstr)
        {
            if (wstr.empty()) return std::string();

            int required = WideCharToMultiByte(CP_THREAD_ACP, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);
            if (0 == required) return std::string();

            std::string str;
            str.resize(required);

            int converted = WideCharToMultiByte(CP_THREAD_ACP, 0, wstr.data(), (int)wstr.size(), &str[0], str.capacity(), NULL, NULL);
            if (0 == converted) return std::string();

            return str;
        }

        /*
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
        */

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

        inline String SetStringColor(const String& str, ColorANSI txtCol, ColorANSI bgCol)
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
