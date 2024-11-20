
#include <iostream>
#include <string>

#define PLATFORM_WINDOWS 1
#include "ShmChat.h"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "expected arguments: ShmChat.exe read|write <name>" << std::endl; return 1;
    }
    std::string modeArg(argv[1]);
    std::string nameArg(argv[2]);
    bool readMode;
    if (modeArg == "read")
    {
        readMode = true;
    }
    else if (modeArg == "write")
    {
        readMode = false;
    }
    else
    {
        std::cerr << "expected arguments: ShmChat.exe read|write <name>" << std::endl; return 1;
    }

    std::cout << "opening '" << nameArg << "' in " << (readMode ? "read" : "write") << " mode" << std::endl;

    auto shmc = shmc_open(nameArg, !readMode);

    if (!shmc)
    {
        std::cerr << "unable to open handle" << std::endl; return 1;
    }

    while (true)
    {
        if (readMode)
        {
            std::string output = shmc_read(shmc);
            if (!output.empty())
            {
                std::cout << output;
            }
#ifdef PLATFORM_WINDOWS
            Sleep(5);
#endif
        }
        else
        {
            std::string input;
            std::getline(std::cin, input);
            shmc_println(shmc, input);
        }
    }

    return 0;
}