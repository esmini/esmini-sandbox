#include "libpath.h"
#include <iostream>

#if defined(_WIN32) || defined(__CYGWIN__)
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
#include <iostream>

std::string GetLibraryPathMain()
{
#if defined(_WIN32)
    char    path[MAX_PATH];
    HMODULE hModule = nullptr;

    // Get the handle of the current module (library)
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)GetLibraryPathMain, &hModule))
    {
        std::cout << "Failed to get library handle.\n";
        return "";
    }

    // Get the full path of the library
    if (GetModuleFileName(hModule, path, MAX_PATH) == 0)
    {
        std::cout <<"Failed to get library path.\n";
        return "";
    }
    //LOG_DEBUG("Library path: {}", path);
    std::string strPath(path);
    return strPath;

#else

    Dl_info dl_info;

    // Use dladdr to retrieve the path of the loaded library
    if (dladdr((void*)&GetLibraryPathMain, &dl_info) == 0)
    {
        throw std::runtime_error("Failed to get library path.");
    }

    return dl_info.dli_fname;

#endif
}


std::string GetDefaultPathMain()
{
    char buffer[1024];
    try
    {
#if defined(_WIN32) || defined(__CYGWIN__)

        DWORD length = GetModuleFileNameA(nullptr, buffer, sizeof(buffer));
        if (length == 0 || length == sizeof(buffer))
        {
            throw std::runtime_error("Failed to get executable path on Windows");
        }
        std::cout <<"Application path: "<< std::string(buffer) << std::endl;
        std::string libraryPath = GetLibraryPathMain();
        if (libraryPath.empty())
        {
            std::cout << "Unable to find the path func\n" ;
        }
        else
        {
            std::cout << "Library path:" << libraryPath << std::endl;
        }
#elif defined(__linux__)
        ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (length == -1)
        {
            throw std::runtime_error("Failed to get executable path on Linux");
        }
        buffer[length] = '\0';  // Null-terminate the string

        std::cout << "Application path: " << std::string(buffer) << std::endl;

        std::string libraryPath = GetLibraryPathMain();
        std::cout << "Library path: "<< libraryPath << std::endl;

#elif defined(__APPLE__)

        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) != 0)
        {
            throw std::runtime_error("Buffer size too small for executable path on macOS");
        }
        std::cout <<"Application path: "<< std::string(buffer) << std::endl;
        std::string libraryPath = GetLibraryPathMain();
        std::cout <<"Library path: "<< libraryPath << std::endl;
#else
        throw std::runtime_error("Unsupported platform");
#endif

        // return std::string(buffer);
        return libraryPath;
    }
    catch (const std::exception& e)
    {
        std::cout <<"Error while getting default path: "<< e.what() << std::endl;
    }
    return "";
}



int main()
{
    std::cout << "-------------code is written in executable------------------" << std::endl;
    std::string path = GetDefaultPathMain();
    std::cout << "Default path from executable: " << path << std::endl;
    std::cout << "-------------code is written in lib------------------" << std::endl;
    path = GetDefaultPath();
    std::cout << "Default path from lib: " << path << std::endl;
    return 0;
}
