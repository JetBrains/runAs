#pragma once
#include <string>
#include <list>

typedef wstring LogLevel;
#define LOG_LEVEL_OFF			TEXT("off")
#define LOG_LEVEL_ERRORS		TEXT("errors")
#define LOG_LEVEL_NORMAL		TEXT("normal")
#define LOG_LEVEL_DEBUG			TEXT("debug")

const list<LogLevel> LogLevels = { LOG_LEVEL_OFF, LOG_LEVEL_ERRORS, LOG_LEVEL_NORMAL, LOG_LEVEL_DEBUG };