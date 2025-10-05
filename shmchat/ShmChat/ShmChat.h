#pragma once

#ifdef PLATFORM_WINDOWS
#define PLATFORM_DEFINED
#endif
#ifdef PLATFORM_LINUX
#define PLATFORM_DEFINED
#endif

#ifndef PLATFORM_DEFINED
#pragma message("ERROR: Please define either 'PLATFORM_WINDOWS' or 'PLATFORM_LINUX' before including ShmChat.h depending on your target platform")
static_assert(false, "ERROR: Please define either 'PLATFORM_WINDOWS' or 'PLATFORM_LINUX' before including ShmChat.h depending on your target platform")
#endif

#include <string>

#pragma pack(push, 1)
struct ShmChat
{
	int32_t size;
	int32_t rIdx;
	int32_t wIdx;
	int32_t lock;
	char buffer[1];
};
#pragma pack(pop)

ShmChat* shmc_open(const std::string &name, bool create = true);
void shmc_print(ShmChat* shmc, const std::string &line);
void shmc_println(ShmChat* shmc, const std::string &line);
std::string shmc_read(ShmChat* shmc);



