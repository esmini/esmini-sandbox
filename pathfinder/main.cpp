#include "libpath.h"

#include <iostream>
#if defined(_WIN32) || defined(__CYGWIN__)
    #include <windows.h>
#elif defined(__linux__)
    #include <unistd.h>
    #include <limits.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
#endif

int main()
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
        std::cout << "Application path: " << std::string(buffer) << std::endl;

        HMODULE const Dll = LoadLibraryExW(L"libpath.dll", nullptr, 0);
        if (!Dll)
        {
            std::cerr << "Failed to load DLL from: libpath.dll" << std::endl;
            return 1;
        }

        using PathType = char const* (__cdecl*) ();

        PathType const path = reinterpret_cast<PathType> (GetProcAddress(Dll, "getLibraryPath"));
        if( !path)
            std::cout << "unable to find the path func" << std::endl;
        else
            std::cout << "Library path: " << std::string(path()) << std::endl;
#elif defined(__linux__)
        ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
        if (length == -1)
        {
            throw std::runtime_error("Failed to get executable path on Linux");
        }
        buffer[length] = '\0'; // Null-terminate the string

        std::cout << "Application path: " << std::string(buffer) << std::endl;

        std::string libraryPath = getLibraryPath();
        std::cout << "Library path: " << libraryPath << std::endl;
#elif defined(__APPLE__)

        uint32_t size = sizeof(buffer);
        if (_NSGetExecutablePath(buffer, &size) != 0)
        {
            throw std::runtime_error("Buffer size too small for executable path on macOS");
        }
        std::cout << "Application path: " << std::string(buffer) << std::endl;

        std::string libraryPath = getLibraryPath();
        std::cout << "Library path: " << libraryPath << std::endl;
#else
    throw std::runtime_error("Unsupported platform");
#endif

    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
