#include "libpath.h"
#include <iostream>

int main()
{

    std::cout << "-------------code is written in lib------------------" << std::endl;
    std::string path = GetLibraryPath();
    std::cout << "Default path from lib: " << path << std::endl;
    // std::string path = GetDefaultPathMain();
    // std::cout << "Default path from executable: " << path << std::endl;
    // std::cout << "-------------code is written in lib------------------" << std::endl;
    // path = GetDefaultPath();
    // std::cout << "Default path from lib: " << path << std::endl;
    return 0;
}