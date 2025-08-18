#ifndef SYSTEM_H
#define SYSTEM_H

// C++ standard library headers
#include <atomic>
#include <chrono>
#include <cstdio> // For sprintf_s
#include <iomanip>
#include <iostream>
#include <limits>
#include <thread>

// System-specific headers
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h> // For ITaskbarList3

// Project local headers
#include "Config.h"
#include "MyLogger.h"
#include "clickscript.h"

class Lights; // Forward declaration for friend class

extern std::atomic<bool> g_isRunning;
extern std::atomic<int> g_currentProgress;
extern std::atomic<int> g_totalProgress;
extern std::atomic<bool> g_emergencyStop; // Global emergency stop flag

// Global variables for taskbar progress
extern ITaskbarList3 *g_pTaskbarList;
extern HWND g_consoleWindow;

class System
{
public:
    void initialize();
    void printMainMenu();
    void runMainLoop();               // Main loop
    int getUserChoice();              // Get user input
    void executeChoice(int choice);   // Execute corresponding task
    void countdown(int seconds = 10); // Countdown timer

    void startAutoclickScript(); // Task 1
    void measureMousePosition(); // Task 2 Measure mouse position
    void temporaryTask();        // Task 99 Test only

    void printSplitLine();

    Config &getConfig() { return config; } // Accessor for config

    friend class Lights; // Allow Lights class to access private members of System

    // Taskbar progress
    bool initializeTaskbarProgress();
    void updateTaskbarProgress(int completed, int total);
    void setTaskbarProgressState(TBPFLAG state);
    void cleanupTaskbarProgress();

    // Emergency stop
    void escapeKeyListener();

private:
    Config config; // Configuration object
};

#endif // SYSTEM_H