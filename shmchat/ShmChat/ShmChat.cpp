// ShmChat.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>

#include "ShmChat.h"

#if 1 // WINDOWS_BUILD
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#else // LINUX_BUILD
#endif

#define BUFFER_SIZE 1024 * 8
#define buffer_t LPCTSTR

void shmc_println(ShmChat* shmc, std::string line)
{
    shmc_print(shmc, line + "\n");
}

void shmc_print(ShmChat* shmc, std::string line)
{
    if (!shmc)
    {
        std::cerr << "ShmChat error: handle is null" << std::endl;
        return;
    }

#if 1 // WINDOWS_BUILD
    while (shmc->lock != 0)
    {
        Sleep(1);
    }
    shmc->lock = 1;

    auto data = line.c_str();
    auto len = line.length();

    auto writeLength = len;
    if (len > shmc->size) writeLength = shmc->size;


    const char* srcStart = &data[len - writeLength];
    //const char* srcEnd = &data[len - 1];

    // calc cap left
    uint32_t cap;
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
#else // LINUX_BUILD
#endif
}

std::string shmc_read(ShmChat *shmc)
{
    std::string result = "";
    if (!shmc)
    {
        std::cerr << "ShmChat error: handle is null" << std::endl;
        return result;
    }

    

#if 1 // WINDOWS_BUILD
    // acquire lock
    while (shmc->lock != 0)
    {
        Sleep(1);
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
        Sleep(1);
    }
    shmc->rIdx = shmc->wIdx;

    // release lock
    shmc->lock = 0;
#else
#endif
	return result;
}

ShmChat* shmc_open(std::string name, bool create)
{
    int32_t fileSize = sizeof(ShmChat) - sizeof(ShmChat::buffer) + BUFFER_SIZE;

#if 1 // WINDOWS_BUILD

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
        FILE_MAP_ALL_ACCESS, // read/write permission
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

    ShmChat* result = (ShmChat*)pBuf;
    
    if (create)
    {
        result->size = BUFFER_SIZE;
        result->rIdx = 0;
        result->wIdx = 0;
        result->lock = 0;
        memset(result->buffer, 0, result->size);
    }

    //CopyMemory((PVOID)pBuf, szMsg, (_tcslen(szMsg) * sizeof(TCHAR)));
    //_getch();

    return result;
#endif
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
