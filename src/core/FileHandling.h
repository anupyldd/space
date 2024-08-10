

#if defined(_WIN64) || defined(_WIN32)
#include "WinDef.h"
#enif

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include "Logging.h"
#include "Defines.h"

#include <filesystem>

// API ---------------------------------

namespace spc
{
    namespace file
    {
        using std::filesystem = fs;

        struct File;    // common struct for both windows and posix files
        enum class FileMode { READ, WRITE, READ_WRITE };
        
        void    Open(const fs::path& path, FileMode mode, File& file); // opens into "file" object
        void    Map(File& file);
        void    CleanUp(File& file);
        String  GetContent(const File& file);
    }
}

// -------------------------------------

namespace spc
{
    namespace file
    {
        struct File
        {
            bool m_valid = false;

            PLATFORM_TYPE(HANDLE, int)          m_fileHandle;
            PLATFORM_TYPE(DWORD,  struct stat)  m_fileStat;
            PLATFORM_TYPE(LPVOID, char*)        m_map;
            HANDLE                              m_fileMapping;      // only used on windows, since there's a 2-step mapping process there
        };
    }
}