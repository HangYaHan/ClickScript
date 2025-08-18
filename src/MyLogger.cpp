#include "MyLogger.h"

// Singleton instance access
MyLogger &MyLogger::getInstance()
{
    static MyLogger instance;
    return instance;
}

// Set log file path
void MyLogger::setLogFile(const std::string &filePath)
{
    std::lock_guard<std::mutex> lock(logMutex);
#if OVERWRITE_LOG_FILE
    logFile.open(filePath, std::ios::out | std::ios::trunc); // Overwrite the log file
#else
    logFile.open(filePath, std::ios::out | std::ios::app); // Append to the log file
#endif
    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file: " << filePath << std::endl;
    }
}

// Set the minimum log level to output
void MyLogger::setLogLevel(LogLevel level)
{
    std::lock_guard<std::mutex> lock(logMutex);
    currentLogLevel = level;
}

// Log a message with a specific log level
void MyLogger::log(LogLevel level, const std::string &message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    if (level < currentLogLevel)
    {
        return;
    }

    std::string logMessage = "[" + logLevelToString(level) + "] " + message;

    // Add timestamp
    std::time_t now = std::time(nullptr);
    char timeBuffer[20];
    std::strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    logMessage = std::string(timeBuffer) + " " + logMessage;

    if (logFile.is_open())
    {
        logFile << logMessage << std::endl;
    }
    else
    {
        std::cerr << logMessage << std::endl;
    }
}

// Convenience methods for specific log levels
void MyLogger::info(const std::string &message)
{
    log(LogLevel::LOG_INFO, message);
}

void MyLogger::warning(const std::string &message)
{
    log(LogLevel::LOG_WARNING, message);
}

void MyLogger::error(const std::string &message)
{
    log(LogLevel::LOG_ERROR, message);
}

void MyLogger::debug(const std::string &message)
{
    log(LogLevel::LOG_DEBUG, message);
}

// Private constructor for singleton
MyLogger::MyLogger() : currentLogLevel(LogLevel::LOG_INFO) {}

// Destructor
MyLogger::~MyLogger()
{
    if (logFile.is_open())
    {
        log(LogLevel::LOG_INFO, "System closing, closing log file.");
        logFile.close();
    }
}

// Helper to convert LogLevel to string
std::string MyLogger::logLevelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::LOG_INFO:
        return "INFO";
    case LogLevel::LOG_WARNING:
        return "WARNING";
    case LogLevel::LOG_ERROR:
        return "ERROR";
    case LogLevel::LOG_DEBUG:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}