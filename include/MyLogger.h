#ifndef MyLogger_H
#define MyLogger_H

#include <windows.h>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>

#define OVERWRITE_LOG_FILE 1 // overwrite the log file on each run

class MyLogger
{
public:
    enum class LogLevel
    {
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR
    };

    // Singleton instance access
    static MyLogger &getInstance();

    // Set log file path
    void setLogFile(const std::string &filePath);

    // Set the minimum log level to output
    void setLogLevel(LogLevel level);

    // Log a message with a specific log level
    void log(LogLevel level, const std::string &message);

    // Convenience methods for specific log levels
    void info(const std::string &message);
    void warning(const std::string &message);
    void error(const std::string &message);
    void debug(const std::string &message);

    // Split line for better readability in logs
    void splitLine()
    {
        log(LogLevel::LOG_INFO, "----------------------------------------------");
    }

private:
    MyLogger(); // Private constructor for singleton
    ~MyLogger();
    MyLogger(const MyLogger &) = delete;
    MyLogger &operator=(const MyLogger &) = delete;

    std::ofstream logFile;
    LogLevel currentLogLevel;
    std::mutex logMutex;

    // Helper to convert LogLevel to string
    std::string logLevelToString(LogLevel level);
};

#endif // MyLogger_H