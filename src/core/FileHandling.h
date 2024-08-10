

#if defined(_WIN64) || defined(_WIN32)

#include "WinDef.h"

#elif defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#endif

#include "Logging.h"

#include <filesystem>

// API ---------------------------------

namespace spc
{
    namespace file
    {
        using std::filesystem = fs;

        struct File;    // common type for both windows and posix file mapping
        enum class FileMode { READ, WRITE, READ_WRITE };
        
        void Open(const fs::path& path, FileMode mode, File& file); // opens into "file" object
    }
}

// -------------------------------------

namespace spc
{
    namespace file
    {
        struct File
        {

        };
    }
}