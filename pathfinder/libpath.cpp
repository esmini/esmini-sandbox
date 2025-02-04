#include "libpath.h"

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <dlfcn.h>
#elif defined(__linux__)
    #include <dlfcn.h>
    #include <unistd.h>
    #include <limits.h>
#endif

#include <stdexcept>

extern "C" const char * getLibraryPath()
{
#if defined(_WIN32)
    char path[MAX_PATH];
    HMODULE hModule = nullptr;

    // Get the handle of the current module (library)
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)getLibraryPath, &hModule))
    {
        throw std::runtime_error("Failed to get library handle.");
    }

    // Get the full path of the library
    if (GetModuleFileName(hModule, path, MAX_PATH) == 0)
    {
        throw std::runtime_error("Failed to get library path.");
    }

    return path;

#else

    Dl_info dl_info;

    // Use dladdr to retrieve the path of the loaded library
    if (dladdr((void*)&getLibraryPath, &dl_info) == 0)
    {
        throw std::runtime_error("Failed to get library path.");
    }

    return dl_info.dli_fname;

#endif
}
