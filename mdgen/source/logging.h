#pragma once

#include "settings.h"

/**
 * Logs arguments with level = E_DEFAULT (can be suppressed by level <= -1)
 */
#define log(_args) { if(Settings::verbose >= Settings::LogLevel::E_DEFAULT) { std::cout << _args << std::endl; } }

/**
 * Logs arguments as error (only suppressed by level <= -2)
 */
#define log_err(_args) { if(Settings::verbose >= Settings::LogLevel::E_DEFAULT - 1) { std::cerr << _args << std::endl; } }

/**
 * Logs arguments with level = E_DETAILED
 */
#define logd(_args) { if(Settings::verbose >= Settings::LogLevel::E_DETAILED) { std::cout << _args << std::endl; } }

/**
 * Logs arguments with level = E_VERBOSE
 */
#define logv(_args) { if(Settings::verbose >= Settings::LogLevel::E_VERBOSE) { std::cout << _args << std::endl; } }



