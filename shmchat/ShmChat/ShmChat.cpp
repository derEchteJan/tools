// ShmChat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

#include "ShmChat.h"

#if WINDOWS_BUILD
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#else // LINUX_BUILD
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#endif

#define BUFFER_SIZE 1024 * 8
#define buffer_t LPCTSTR

static void shmc_sleep();


void shmc_println(ShmChat* shmc, const std::string &line)
{
    shmc_print(shmc, line + "\n");
}

void shmc_print(ShmChat* shmc, const std::string &line)
{
    if (!shmc)
    {
        std::cerr << "ShmChat error: handle is null" << std::endl;
        return;
    }

    while (shmc->lock != 0)
    {
        shmc_sleep();
    }
    shmc->lock = 1;

    auto data = line.c_str();
    auto len = line.length();

    auto writeLength = len;
    if (len > shmc->size) writeLength = shmc->size;


    const char* srcStart = &data[len - writeLength];
    //const char* srcEnd = &data[len - 1];

    // calc cap left
    long cap;
    if (shmc->wIdx >= shmc->rIdx)
    {
        cap = shmc->size - (shmc->wIdx - shmc->rIdx);
    }
    else
    {
        cap = shmc->rIdx - shmc->wIdx;
    }
    
    // copy data
    if (writeLength + shmc->wIdx <= shmc->size) // no wrap around
    {
        char* dst = shmc->buffer + shmc->wIdx;
        memcpy(dst, srcStart, writeLength);
    }
    else // wrap around
    {
        char* dst = shmc->buffer + shmc->wIdx;
        auto firstLen = shmc->size - shmc->wIdx;
        memcpy(dst, srcStart, firstLen);
        auto secondLen = writeLength - firstLen;
        dst = shmc->buffer;
        memcpy(dst, srcStart + firstLen, secondLen);
    }

    // update indices
    shmc->wIdx += writeLength;
    shmc->wIdx %= shmc->size;
    if (writeLength >= cap)
    {
        shmc->rIdx = (shmc->wIdx + 1) % shmc->size;
    }

    shmc->lock = 0;
}

std::string shmc_read(ShmChat *shmc)
{
    std::string result = "";
    if (!shmc)
    {
        std::cerr << "ShmChat error: handle is null" << std::endl;
        return result;
    }

    // acquire lock
    while (shmc->lock != 0)
    {
        shmc_sleep();
    }
    shmc->lock = 1;

    if(shmc->rIdx < shmc->wIdx)
    {
        auto len = shmc->wIdx - shmc->rIdx;
        result.assign(&shmc->buffer[shmc->rIdx], len);
    }
    else if (shmc->rIdx > shmc->wIdx)
    {
        std::string firstPart;
        std::string secondPart;
        auto firstLen = shmc->size - shmc->rIdx;
        auto secondLen = shmc->wIdx;
        firstPart.assign(&shmc->buffer[shmc->rIdx], firstLen);
        secondPart.assign(&shmc->buffer[0], secondLen);
        result = firstPart + secondPart;
    }
    else // empty
    {
        shmc_sleep();
    }
    shmc->rIdx = shmc->wIdx;

    // release lock
    shmc->lock = 0;

	return result;
}

ShmChat* shmc_open(const std::string &name, bool create) // MARK JAN: TODO - 'create' -> auto determine, do not pass this in
{
    ShmChat* result = nullptr;
    int32_t fileSize = sizeof(ShmChat) - sizeof(ShmChat::buffer) + BUFFER_SIZE;

#if WINDOWS_BUILD

    HANDLE hMapFile;
    LPCTSTR pBuf;
    LPCSTR szName = name.c_str();

    hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,    // use paging file
        NULL,                    // default security
        PAGE_READWRITE,          // read/write access
        0,                       // maximum object size (high-order DWORD)
        fileSize,                // maximum object size (low-order DWORD)
        szName);                 // name of mapping object

    if (hMapFile == NULL)
    {
        _tprintf(TEXT("Could not create file mapping object (%d).\n"),
            GetLastError());
        return nullptr;
    }
    pBuf = (LPTSTR)MapViewOfFile(hMapFile,   // handle to map object
        FILE_MAP_ALL_ACCESS,                 // read/write permission
        0,
        0,
        fileSize);

    if (pBuf == NULL)
    {
        _tprintf(TEXT("Could not map view of file (%d).\n"),
            GetLastError());

        CloseHandle(hMapFile);
        return nullptr;
    }

    result = (ShmChat*)pBuf;

#else // LINUX_BUILD

    // open named shm file
    int fd = shm_open(name.c_str(), O_RDWR, 0666);
    create = false;

    if(fd == -1)
    {
        // create if it doesnt yet exist
        fd = shm_open(name.c_str(), O_CREAT | O_RDWR, 0666);
        create = true;
    }

    if(fd == -1)
    {
        std::cerr << "unable to open shm file" << std::endl;
        perror("shmc_open:");
        return nullptr;
    }

    // configure the size of the shm file
    ftruncate(fd, fileSize);

    // map shm file to memory
    result = (ShmChat*)(mmap(0, fileSize, PROT_WRITE, MAP_SHARED, fd, 0));

#endif

    if(!result) return nullptr;

    // initialize state if newly created
    if (create)
    {
        result->size = BUFFER_SIZE;
        result->rIdx = 0;
        result->wIdx = 0;
        result->lock = 0;
        memset(result->buffer, 0, result->size);
    }

    return result;
}

void shmc_sleep()
{
    // brief delay to avoid cpu usage while polling etc
#if WINDOWS_BUILD
    Sleep(1);
#else // LINUX_BUILD
    usleep(1000);
#endif
}