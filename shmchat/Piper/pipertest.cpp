
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    std::cout << "~ piper test ~" << std::endl;
    std::cout << "args:" << std::endl;
    for(int i = 0; i < argc; i++)
    {
        std::cout << " - " << argv[i] << std::endl;
    }
}