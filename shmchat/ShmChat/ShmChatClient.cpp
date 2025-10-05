
#include <iostream>
#include <string>

//#define PLATFORM_WINDOWS
#define PLATFORM_LINUX

#include "ShmChat.h"
#include "ShmChat.cpp"

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <unistd.h>
#endif

static void sleep();

enum Mode
{
    E_Read, E_Write, E_Send, E_Get, E_Invalid
};

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        std::cerr << "expected arguments: shmchat read|write|get|send <name>" << std::endl;
        exit(2);
    }
    std::string modeArg(argv[1]); // arg1: shmc identifier name
    std::string nameArg(argv[2]); // arg2: mode
    
    Mode mode = E_Invalid;

    if (modeArg == "read") mode = E_Read;
    if (modeArg == "write") mode = E_Write;
    if (modeArg == "get") mode = E_Get;     // TODO: do these block? implement max lock loop tries maybe
    if (modeArg == "send") mode = E_Send;   // TODO: does it send a newline after each argument? define a parameter to change behaviour
                                            // TODO: print help

    bool create = mode == E_Write;

    if(mode == E_Invalid)
    {
        std::cerr << "expected arguments: shmchat read|write|get|send <name>" << std::endl;
        exit(2);
    }

    if(mode == E_Read || mode == E_Write)
    {
        std::cout << "opening '" << nameArg << "' in " << modeArg << " mode" << std::endl;
    }

    auto shmc = shmc_open(nameArg, create);

    if (!shmc)
    {
        std::cerr << "unable to open shmc handle" << std::endl;
        perror("shmc:");
        exit(1);
    }


    // --- RUN MODE ACTION ---

    // loop continously, read from stdin and send it to shmc
    if(mode == E_Read)
    {
        while(true)
        {
            std::string output = shmc_read(shmc);
            if (!output.empty())
            {
                std::cout << output;
            }
            sleep();
        }
    }
    // loop continously, read from shmc and print to stdout
    if(mode == E_Write)
    {
        while(true)
        {
            std::string input;
            std::getline(std::cin, input);
            shmc_println(shmc, input);
            sleep();
        }
    }
    // reads current contents of shmc and sends them to stdout, then exits
    if(mode == E_Get)
    {
        std::string output = shmc_read(shmc);
        if (!output.empty())
        {
            std::cout << output;
        }
    }
    // sends arguments to shmc then exits
    if(mode == E_Send)
    {
        for(int argi = 3; argi < argc; argi++)
        {
            char *arg = argv[argi];
            shmc_println(shmc, std::string(arg));
        }
    }

    return 0;
}

static void sleep()
{
#ifdef PLATFORM_WINDOWS
    Sleep(5);
#else
    usleep(1000);
#endif
}