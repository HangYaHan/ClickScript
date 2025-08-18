#ifndef THREADMANAGER_H
#define THREADMANAGER_H

// C++ standard library headers
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

// System-specific headers
#include <windows.h>

// Project local headers
#include "MyLogger.h"

class ThreadManager
{
public:
    static ThreadManager &getInstance();

    // Emergency stop control
    void setEmergencyStop(bool stop);
    bool isEmergencyStop() const;

    // Running state
    void setRunning(bool running);
    bool isRunning() const;

    // Progress information
    void setProgress(int current, int total);
    int getCurrentProgress() const;
    int getTotalProgress() const;

    // Thread management
    void startEmergencyListener();
    void stopEmergencyListener();

    // Reset state
    void reset();

private:
    ThreadManager() = default;
    ~ThreadManager();

    ThreadManager(const ThreadManager &) = delete;
    ThreadManager &operator=(const ThreadManager &) = delete;

    // Emergency stop listener thread
    void emergencyListenerThread();

    // Atomic variables
    std::atomic<bool> emergencyStop{false};
    std::atomic<bool> isRunningState{false};
    std::atomic<int> currentProgress{0};
    std::atomic<int> totalProgress{0};

    // Thread management
    std::thread emergencyThread;
    std::atomic<bool> shouldStopListener{false};

    mutable std::mutex mtx;
};

#endif // THREADMANAGER_H