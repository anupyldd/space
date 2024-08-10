
namespace spc
{

#if defined(_WIN64) || defined(_WIN32)
    // Windows ----------

    #include "WinDef.h"

    namespace con
    {
        inline std::wstring Utf8ToUtf16(const String& str8) {
            int sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, str8.c_str(), (int)str8.size(), NULL, 0);
            std::wstring str16(sizeNeeded, 0);
            MultiByteToWideChar(CP_UTF8, 0, str8.c_str(), (int)str8.size(), &str16[0], sizeNeeded);
            return str16;
        }

        inline String Utf16ToUtf8(const std::wstring& str16) {
            int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str16.c_str(), (int)str16.size(), NULL, 0, NULL, NULL);
            String str8(sizeNeeded, 0);
            WideCharToMultiByte(CP_UTF8, 0, str16.c_str(), (int)str16.size(), &str8[0], sizeNeeded, NULL, NULL);
            return str8;
        }

        inline void Print(const String& str) {
            std::wcout << Utf8ToUtf16(str);
        }
        inline void PrintN(const String& str) {
            std::wcout << Utf8ToUtf16(str) << L'\n';
        }
        inline void Input(String& str) {
            std::wstring wstr;
            std::wcin >> wstr;
            str = Utf16ToUtf8(wstr);
        }
    }

#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
    // Unix -------------

    namespace con
    {
        inline void Print(const String& str) { std::cout << str; }
        inline void PrintN(const String& str) { std::cout << str << '\n'; }
        inline void Input(String& str) { std::cin >> str; }
    }

#else

#error "Platform not supported"

#endif

}
}