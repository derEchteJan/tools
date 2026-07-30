#include "benchmark.h"

#include <chrono>
#include <iostream>

#include "logging.h"

static std::chrono::steady_clock::time_point s_begin;
static std::chrono::steady_clock::time_point s_end;

void benchmark_start()
{
    s_begin = std::chrono::steady_clock::now();
}

void benchmark_end()
{
    s_end = std::chrono::steady_clock::now();
    logd( "Runtime: " << std::chrono::duration_cast<std::chrono::microseconds>(s_end - s_begin).count() << " us" );
}



