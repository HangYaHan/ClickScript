#include "system.h"

// Global variables
std::atomic<bool> g_isRunning{false};
std::atomic<int> g_currentProgress{0};
std::atomic<int> g_totalProgress{10};
ITaskbarList3 *g_pTaskbarList = nullptr;
HWND g_consoleWindow = nullptr;

// Global emergency stop flag
std::atomic<bool> g_emergencyStop{false};

void System::initialize()
{
    std::cout << "Initializing system..." << std::endl;
    MyLogger::getInstance().setLogFile("system.log");
    MyLogger::getInstance().setLogLevel(MyLogger::LogLevel::LOG_DEBUG);

    MyLogger::getInstance().info("Running initialization...");
    if (this->config.load())
    {
        MyLogger::getInstance().debug("Configuration loaded successfully.");
    }
    else
    {
        MyLogger::getInstance().debug("Failed to load configuration.");
        std::cout << "Failed to load configuration, creating an empty one..." << std::endl;
    }

    MyLogger::getInstance().debug("System initialization finished.");
    // Perform system initialization tasks
}

void System::runMainLoop()
{
    while (true)
    {
        printMainMenu();
        int choice = getUserChoice();

        if (choice == 0)
        {
            std::cout << "Exiting program..." << std::endl;
            MyLogger::getInstance().info("Program exited by user.");
            break;
        }

        executeChoice(choice);
    }
}

int System::getUserChoice()
{
    int choice;
    std::cout << "Enter your choice (0 to exit): ";
    std::cin >> choice;

    // Clear the input buffer
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    return choice;
}

void System::executeChoice(int choice)
{
    switch (choice)
    {
    case 0:
        system("cls");
        temporaryTask();
        break;
    case 1:
        system("cls");
        startAutoclickScript();
        break;
    case 2:
        system("cls");
        measureMousePosition();
        break;
    case 3:
        system("cls");
        configInit();
        break;
    case 99:
        system("cls");
        temporaryTask();
        break;
    default:
        system("cls");
        std::cout << "Invalid choice. Please try again." << std::endl;
        break;
    }

    // Pause to let the user see the result
    std::cout << "\nPress Enter to continue...";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void System::startAutoclickScript()
{
    MyLogger::getInstance().splitLine();
    MyLogger::getInstance().info("Autoclick script started.");

    // === Load ClickScript ===

    ClickScript ClickScript;

    std::cout << "Please enter the task to be loaded (e.g., task1.clk --- default: task.clk): ";
    std::string filename;
    std::getline(std::cin, filename);
    if (filename.empty())
    {
        filename = "task.clk";
        std::cout << "No input detected. Using default: " << filename << std::endl;
    }

    int loops = 0;
    std::string path1 = config.get("PATH_1");
    std::string path2 = config.get("PATH_2");

    ClickScript.load_ClickScript_fromfile(filename);
    loops = ClickScript.get_loops();
    ClickScript.print_ClickScript();
    std::cout << "-----------------------------" << std::endl;
    config.print();
    std::cout << "-----------------------------" << std::endl;
    std::cout << "Press Enter to confirm and start" << std::endl;
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Get console window handle
    g_consoleWindow = GetConsoleWindow();
    if (g_consoleWindow == NULL)
    {
        std::cerr << "Failed to get console window handle!" << std::endl;
        MyLogger::getInstance().error("Failed to get console window handle!");
        return;
    }

    // Initialize taskbar progress interface
    bool taskbarInitialized = initializeTaskbarProgress();
    if (!taskbarInitialized)
    {
        std::cerr << "Failed to initialize taskbar progress!" << std::endl;
        MyLogger::getInstance().error("Failed to initialize taskbar progress!");
        MyLogger::getInstance().warning("Continuing without taskbar progress.");
        // Continue execution without progress bar
    }

    // ===== Initialize emergency stop listener =====
    // Reset emergency stop flag
    g_emergencyStop.store(false);

    // Start ESC key listener thread
    std::thread escapeThread(&System::escapeKeyListener, this);
    escapeThread.detach(); // Detach thread to run in background

    std::cout << std::endl
              << "=== EMERGENCY STOP ENABLED ===" << std::endl;
    std::cout << "Press ESC key at any time to immediately stop the procedure!" << std::endl;
    MyLogger::getInstance().info("Emergency stop monitor activated - Press ESC to stop");

    // Set total progress and current progress
    g_totalProgress.store(loops);
    g_currentProgress.store(0);

    // Set running flag
    g_isRunning.store(true);

    // Set taskbar progress state to normal (green)
    if (taskbarInitialized)
    {
        setTaskbarProgressState(TBPF_NORMAL);
    }

    std::cout << "ClickScript procedure will execute " << loops << " rounds." << std::endl
              << std::endl;

    MyLogger::getInstance().splitLine();
    MyLogger::getInstance().info("ClickScript procedure will execute " + std::to_string(loops) + " rounds.");

    // ===== Execute loop and update progress =====
    std::cout << "Press Enter to start after 3 seconds...";
    getchar();
    countdown(3);
    bool completedNormally = true;

    for (int i = 0; i < loops; i++)
    {
        // ===== Check emergency stop flag =====
        if (g_emergencyStop.load())
        {
            std::cout << "\n!!! EMERGENCY STOP TRIGGERED !!!" << std::endl;
            std::cout << "ClickScript procedure stopped by user (ESC key)!" << std::endl;
            MyLogger::getInstance().warning("ClickScript procedure emergency stopped at round " + std::to_string(i + 1));

            // Set progress bar to error state (red)
            if (taskbarInitialized)
            {
                setTaskbarProgressState(TBPF_ERROR);
            }

            completedNormally = false;
            break;
        }

        // Check normal interrupt flag
        if (!g_isRunning.load())
        {
            std::cout << "ClickScript procedure interrupted!" << std::endl;
            MyLogger::getInstance().info("ClickScript procedure interrupted at round " + std::to_string(i + 1));

            if (taskbarInitialized)
            {
                setTaskbarProgressState(TBPF_PAUSED);
            }

            completedNormally = false;
            break;
        }

        std::cout << "=== Executing ClickScript round " << (i + 1) << " of " << loops << " ===" << std::endl;
        std::cout << "Press ESC to emergency stop..." << std::endl;

        MyLogger::getInstance().debug("=== Executing ClickScript round " + std::to_string(i + 1) + " of " + std::to_string(loops) + " ===");

        // Update progress
        g_currentProgress.store(i + 1);
        if (taskbarInitialized)
        {
            updateTaskbarProgress(i + 1, loops);
        }

        // Execute click script (this may take a long time, should support emergency stop inside)
        if (!g_emergencyStop.load() && g_isRunning.load())
        {
            ClickScript.execute();
        }
        else
        {
            break; // Emergency stop check
        }

        if (config.get("Number_of_Files_Check") == "ENABLE")
        {
            while (ClickScript.count_FilesInPath(path1) > ClickScript.count_FilesInPath(path2))
            {
                std::cout << "---" << std::endl;
                std::cout << "Warning: Path1 has more files than Path2." << std::endl;
                std::cout << "Execute auto-delete." << std::endl;
                ClickScript.deleteLatestFileInPath(path1);
            }
            while (ClickScript.count_FilesInPath(path1) < ClickScript.count_FilesInPath(path2))
            {
                std::cout << "---" << std::endl;
                std::cout << "Warning: Path2 has more files than Path1." << std::endl;
                std::cout << "Execute auto-delete." << std::endl;
                ClickScript.deleteLatestFileInPath(path2);
            }
        }
    }
    // ===== Completion handling =====
    if (completedNormally && g_isRunning.load() && !g_emergencyStop.load())
    {
        std::cout << "\n=== ALL ROUNDS COMPLETED SUCCESSFULLY! ===" << std::endl;
        MyLogger::getInstance().info("All rounds completed successfully!");

        // Set progress bar to completed state (green, 100%)
        if (taskbarInitialized)
        {
            setTaskbarProgressState(TBPF_NORMAL);
            updateTaskbarProgress(loops, loops);

            // Keep completed state visible for 3 seconds
            std::cout << "Keeping progress bar visible for 3 seconds..." << std::endl;
            Sleep(3000);
        }
    }
    else if (g_emergencyStop.load())
    {
        std::cout << "\n=== PROCEDURE TERMINATED BY EMERGENCY STOP ===" << std::endl;
        MyLogger::getInstance().warning("Procedure terminated by emergency stop");

        // Keep error state visible for 3 seconds
        if (taskbarInitialized)
        {
            std::cout << "Emergency stop indication on taskbar for 3 seconds..." << std::endl;
            Sleep(3000);
        }
    }

    // ===== Cleanup progress bar =====
    if (taskbarInitialized)
    {
        setTaskbarProgressState(TBPF_NOPROGRESS);
    }
    SetConsoleTitle("ClickScript - Ready");

    // Stop all running flags
    g_isRunning.store(false);
    g_emergencyStop.store(true); // Notify emergency stop thread to exit

    // Give emergency stop thread some time to exit naturally
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    // Cleanup COM interface
    if (taskbarInitialized)
    {
        cleanupTaskbarProgress();
    }

    std::cout << "ClickScript procedure completed." << std::endl;
    MyLogger::getInstance().info("ClickScript procedure completed.");
    MyLogger::getInstance().splitLine();

    // Reset console window handle
    g_consoleWindow = nullptr;
    // Reset taskbar list interface
    if (g_pTaskbarList)
    {
        g_pTaskbarList->Release();
        g_pTaskbarList = nullptr;
    }
    // Uninitialize COM
    CoUninitialize();
    MyLogger::getInstance().debug("Resources cleaned up.");
    MyLogger::getInstance().info("Autoclick script completed.");
}

void System::printMainMenu()
{
    system("cls");
    printSplitLine();
    std::cout << "Main Menu" << std::endl;
    printSplitLine();
    std::cout << "1. Start Autoclick script." << std::endl;
    std::cout << "2. Measure mouse position." << std::endl;
    std::cout << "3. Show configuration." << std::endl;
    std::cout << "99. Test." << std::endl;
    printSplitLine();
    std::cout << "0. Exit" << std::endl;
    printSplitLine();
}

void System::printSplitLine()
{
    std::cout << "----------------------------------------------" << std::endl;
}

void System::temporaryTask()
{
    system("cls");
    std::cout << "Debug Only" << std::endl;
    std::cout << "No temporary task implemented." << std::endl;
}

void System::countdown(int seconds)
{
    for (int i = seconds; i > 0; --i)
    {
        std::cout << "Countdown: " << i << " seconds remaining..." << std::endl;
        Sleep(1000);
    }
    std::cout << "Countdown finished!" << std::endl;
}

void System::measureMousePosition()
{
    std::cout << "Measuring mouse position. Press 'q' and Enter to quit." << std::endl;
    std::string input;
    while (true)
    {
        std::cout << "Press 'q' and Enter to quit, or just 'Enter' to continue: ";
        std::getline(std::cin, input);
        if (input == "q" || input == "Q")
        {
            break;
        }
        POINT p;
        if (GetCursorPos(&p))
        {
            std::cout << "Current Mouse Position: (" << p.x << ", " << p.y << ")" << std::endl;
        }
        else
        {
            std::cout << "Failed to get mouse position." << std::endl;
        }
    }
}

// Initialize COM and taskbar interface
bool System::initializeTaskbarProgress()
{
    HRESULT hr = CoInitialize(nullptr);
    if (FAILED(hr))
    {
        std::cerr << "Failed to initialize COM" << std::endl;
        return false;
    }

    hr = CoCreateInstance(CLSID_TaskbarList, nullptr, CLSCTX_INPROC_SERVER,
                          IID_ITaskbarList3, (void **)&g_pTaskbarList);
    if (FAILED(hr))
    {
        std::cerr << "Failed to create taskbar list interface" << std::endl;
        CoUninitialize();
        return false;
    }

    hr = g_pTaskbarList->HrInit();
    if (FAILED(hr))
    {
        std::cerr << "Failed to initialize taskbar list" << std::endl;
        g_pTaskbarList->Release();
        g_pTaskbarList = nullptr;
        CoUninitialize();
        return false;
    }

    return true;
}

// Update taskbar progress
void System::updateTaskbarProgress(int current, int total)
{
    if (g_pTaskbarList && g_consoleWindow)
    {
        if (current == 0)
        {
            // Set progress state to normal
            g_pTaskbarList->SetProgressState(g_consoleWindow, TBPF_NORMAL);
        }

        // Update progress value
        g_pTaskbarList->SetProgressValue(g_consoleWindow, current, total);

        // Update window title with progress
        char title[256];
        sprintf_s(title, sizeof(title),
                  "ClickScript - Progress: %d/%d (%.1f%%)",
                  current, total, (float)current / total * 100);
        SetConsoleTitle(title);
    }
}

// Set taskbar progress state
void System::setTaskbarProgressState(TBPFLAG state)
{
    if (g_pTaskbarList && g_consoleWindow)
    {
        g_pTaskbarList->SetProgressState(g_consoleWindow, state);
    }
}

// Cleanup taskbar progress
void System::cleanupTaskbarProgress()
{
    if (g_pTaskbarList)
    {
        if (g_consoleWindow)
        {
            g_pTaskbarList->SetProgressState(g_consoleWindow, TBPF_NOPROGRESS);
        }
        g_pTaskbarList->Release();
        g_pTaskbarList = nullptr;
    }
    CoUninitialize();
}
void System::escapeKeyListener()
{
    while (!g_emergencyStop.load())
    {
        // Check if ESC key is pressed
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            std::cout << "\n=== EMERGENCY STOP ACTIVATED ===\n"
                      << std::endl;
            std::cout << "ESC key detected! Stopping all operations..." << std::endl;

            MyLogger::getInstance().info("Emergency stop activated by ESC key.");

            // Set emergency stop flag
            g_emergencyStop.store(true);

            // Also set system running flag to false
            g_isRunning.store(false);

            // Play system warning sound
            MessageBeep(MB_ICONWARNING);

            break;
        }

        // Short sleep to reduce CPU usage
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Emergency stop monitor thread terminated." << std::endl;
}

void System::configInit()
{
    if (config.load())
    {
        std::cout << "Configuration loaded successfully." << std::endl;
        config.print();
    }
    else
    {
        std::cout << "Failed to load configuration file." << std::endl;
        std::cout << "Creating default configuration file..." << std::endl;
        if (config.create())
        {
            std::cout << "Default configuration file created successfully." << std::endl;
            config.set("Number_of_Files_Check", "DISABLE");
            config.set("PATH_1", ".\\PATH1");
            config.set("PATH_2", ".\\PATH2");
            config.save();
            std::cout << "------------------------------" << std::endl;
            std::cout << "Number_of_Files_Check = DISABLE" << std::endl;
            std::cout << "PATH_1 = .\\PATH1" << std::endl;
            std::cout << "PATH_2 = .\\PATH2" << std::endl;
            std::cout << "------------------------------" << std::endl;
            std::cout << "Configuration file intialized with default values." << std::endl;
            std::cout << "Please edit the configuration file and reload the configuration." << std::endl;
        }
        else
        {
            std::cout << "Failed to create configuration file." << std::endl;
            std::cout << "Fatal error, exiting..." << std::endl;
        }
    }
}
