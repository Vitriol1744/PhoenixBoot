#pragma once

#include <stdint.h>

#include "Utility/libc.hpp"

enum class LogLevel
{
    eTrace,
    eInfo,
    eWarn,
    eError,
    eFatal,
};

class Logger
{
    public:
        static void Log(LogLevel logLevel, const char* message, ...);
        static void Log(LogLevel logLevel, const char* message, va_list args);
        static void SetOutputStreams(OutputStream _outputStreams) { outputStreams = _outputStreams; }
        static OutputStream GetOutputStreams() { return outputStreams; }

    private:
        Logger() = delete;

        static OutputStream outputStreams;
};

#ifdef PH_DEBUG
    #define LOG_TRACE(...)  Logger::Log(LogLevel::eTrace, __VA_ARGS__)
    #define LOG_INFO(...)   Logger::Log(LogLevel::eInfo, __VA_ARGS__)
    #define LOG_WARN(...)   Logger::Log(LogLevel::eWarn, __VA_ARGS__)
    #define LOG_ERROR(...)  Logger::Log(LogLevel::eError, __VA_ARGS__)
    #define LOG_FATAL(...)  Logger::Log(LogLevel::eFatal, __VA_ARGS__)
#else
    #define LOG_TRACE(...)
    #define LOG_INFO(...)
    #define LOG_WARN(...)
    #define LOG_ERROR(...)
    #define LOG_FATAL(...)
#endif