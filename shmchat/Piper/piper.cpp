#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <fcntl.h> 
#include <string.h>

#include <iostream>
#include <string>
#include <algorithm>

#define PLATFORM_LINUX
#include "../ShmChat/ShmChat.h"
#include "../ShmChat/ShmChat.cpp"

#define BEDROCK_SERVER "home/kowalski/bedrock-server-1.21.112.1/bedrock_server"
#define BEDROCK_SERVER_WDIR "home/kowalski/bedrock-server-1.21.112.1"


namespace fv
{

static char s_readBuffer[1024 * 4];

// like std::getline but doesnt block on read operation
// if setNoBlock was configured on the fd
static void getline(int fd, std::string &line)
{
    line = "";
    
    auto bufferSize = sizeof(s_readBuffer);
    memset(s_readBuffer, 0, bufferSize);

    int ret = read(fd, s_readBuffer, bufferSize - 1);
    if(ret > 0)
    {
        s_readBuffer[ret - 1] = '\0';
        line = std::string(s_readBuffer);
    }
}

// sets stream non blocking for read operations
static void setNoBlock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

}

// wrapper class for posix pipe
class Pipe
{
public:
    int m_pipedes[2]; // underlying posix pipe file descriptors, first is input, second output

    Pipe()
    {
        m_pipedes[0] = -1;
        m_pipedes[1] = -1;

        if (pipe(m_pipedes) == -1)
        {
            perror("Pipe: pipe(int*)");
            exit(1);
        }
    }

    int in() { return m_pipedes[0]; }
    int out() { return m_pipedes[1]; }

    void redirectTo(int fd_in, int fd_out)
    {
        dup2(m_pipedes[0], fd_in);
        dup2(m_pipedes[1], fd_out);
    }

    std::string readLine()
    {
        memset(fv::s_readBuffer, 0, sizeof(fv::s_readBuffer));

        ssize_t len = read(in(), fv::s_readBuffer, sizeof(fv::s_readBuffer) - 1);

        return std::string(fv::s_readBuffer);
    }

    void Write(const std::string &text)
    {
        ::write(out(), text.c_str(), text.length());
    }

    void WriteLine(const std::string &line)
    {
        ::write(out(), line.c_str(), line.length());
        ::write(out(), "\r\n", 2);
    }

};


int main(int argc, char **argv)
{   
    pid_t childPid; 
    
    Pipe downPipe; // sends from parent process to child
    Pipe upPipe;   // sends from child process to parent
    
    if(argc != 2)
    {
        std::cerr << "specify executable" << std::endl;
        exit(2);
    }

    childPid = fork(); // Create a child process 
    
    if (childPid == -1)
    { 
        perror("unable to fork process");
        exit(1);
    }
    
    if (childPid == 0)
    {
        // --- CHILD PROCESS ---

        std::string arg1(argv[1]);
        std::string arg2 = "./" + arg1;

        std::cout << "piper running: " << arg1 << std::endl;

        dup2(downPipe.in(), STDIN_FILENO);
        dup2(upPipe.out(), STDOUT_FILENO);

        // TODO: pass in trailing arguments, create duplicate argv

        execl(arg1.c_str(), arg2.c_str(), nullptr);

        std::cerr << "unable to execute '" << arg1 << "'" << std::endl;
        perror("");

        exit(1);
    }
    else
    { 
        // --- PARENT PROCESS ---

        fv::setNoBlock(STDIN_FILENO);
        fv::setNoBlock(upPipe.in());

        auto shmc = shmc_open("piper", true);
        if(!shmc) { std::cerr << "unable to open shmc" << std::endl; exit(1); };
        std::cout << "creates shmc 'piper'" << std::endl;
        
        std::string input;
        std::string response;
        
        while(true)
        {
            // --- SEND INPUT TO CHILD ---

            // read from stdin
            fv::getline(STDIN_FILENO, input); // std::getline still blocks, need to use fv::getline instead
            if(!input.empty())
            {   
                std::cout << "sending (from stdin): '" << input << "'" << std::endl; 
                downPipe.WriteLine(input);
            }

            // read from shmchat
            input = shmc_read(shmc); // TODO: properly remove trailing linebreaks, split string by linebreaks and foreach
            if(!input.empty())
            {
                input.erase(std::remove(input.begin(), input.end(), '\n'), input.cend());
                std::cout << "sending (from shmc): '" << input << "'" << std::endl; 
                downPipe.WriteLine(input);
            }

            // --- READ OUTPUT FROM CHILD ---

            // read from stdout
            response = upPipe.readLine();
            if(!response.empty())
            {
                //std::cout << "response: \n'" << response << "\"" << std::endl;
                std::cout << response << std::endl;
            }

            // TODO: send back to shmc as well

            usleep(10000);
        }

        // TODO: - kill child process
        //       - listen for child process exit

        exit(0);
    } 
} 