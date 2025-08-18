#include "ThreadManager.h"

ThreadManager &ThreadManager::getInstance()
{
    static ThreadManager instance;
    return instance;
}

ThreadManager::~ThreadManager()
{
    stopEmergencyListener();
}

void ThreadManager::setEmergencyStop(bool stop)
{
    emergencyStop.store(stop);
    if (stop)
    {
        MyLogger::getInstance().info("Emergency stop activated!");
        std::cout << "\n*** EMERGENCY STOP ACTIVATED ***" << std::endl;
    }
}

bool ThreadManager::isEmergencyStop() const
{
    return emergencyStop.load();
}

void ThreadManager::setRunning(bool running)
{
    isRunningState.store(running);
}

bool ThreadManager::isRunning() const
{
    return isRunningState.load();
}

void ThreadManager::setProgress(int current, int total)
{
    currentProgress.store(current);
    totalProgress.store(total);
}

int ThreadManager::getCurrentProgress() const
{
    return currentProgress.load();
}

int ThreadManager::getTotalProgress() const
{
    return totalProgress.load();
}

void ThreadManager::startEmergencyListener()
{
    if (!emergencyThread.joinable())
    {
        shouldStopListener.store(false);
        emergencyThread = std::thread(&ThreadManager::emergencyListenerThread, this);
        MyLogger::getInstance().info("Emergency listener thread started");
    }
}

void ThreadManager::stopEmergencyListener()
{
    shouldStopListener.store(true);
    if (emergencyThread.joinable())
    {
        emergencyThread.join();
        MyLogger::getInstance().info("Emergency listener thread stopped");
    }
}

void ThreadManager::reset()
{
    emergencyStop.store(false);
    isRunningState.store(false);
    currentProgress.store(0);
    totalProgress.store(0);
}

void ThreadManager::emergencyListenerThread()
{
    MyLogger::getInstance().debug("Emergency listener thread running");

    while (!shouldStopListener.load())
    {
        // Check ESC key state
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            if (isRunning())
            {
                setEmergencyStop(true);
                break;
            }
        }

        // Display progress (if running)
        if (isRunning())
        {
            int current = getCurrentProgress();
            int total = getTotalProgress();
            if (total > 0)
            {
                // Update console title to show progress
                std::string title = "ISC - Progress: " + std::to_string(current) + "/" + std::to_string(total) + " (Press ESC to stop)";
                SetConsoleTitle(title.c_str());
            }
        }

        // 50ms check interval
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    MyLogger::getInstance().debug("Emergency listener thread exiting");
}