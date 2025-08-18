#include "clickscript.h"

void ClickScript::execute()
{
    for (const auto &behavior : behaviors)
    {
        switch (behavior.action)
        {
        case LEFT_CLICK:
            // Simulate left click at behavior.point
            simulateLeftClick(behavior.point);
            break;
        case RIGHT_CLICK:
            // Simulate right click at behavior.point
            simulateRightClick(behavior.point);
            break;
        case ENTER_KEY:
            // Simulate pressing enter key
            simulateEnterKey(behavior.key);
            break;
        case DELAY:
            // Wait for behavior.delay milliseconds
            simulateDelay(behavior.delay);
            break;
        case NONE:
        default:
            // Do nothing
            MyLogger::getInstance().warning("Unknown action in ClickScript.");
            break;
        }
    }
}

void ClickScript::simulateLeftClick(const Point &point)
{
    // Implementation for simulating a left click at the specified point
    MyLogger::getInstance().debug("Simulating left click at (" + std::to_string(point.x) + ", " + std::to_string(point.y) + ")");
    SetCursorPos(point.x, point.y); // Move cursor to the point
    mouse_event(MOUSEEVENTF_LEFTDOWN, point.x, point.y, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, point.x, point.y, 0, 0);
}

void ClickScript::simulateRightClick(const Point &point)
{
    // Implementation for simulating a right click at the specified point
    MyLogger::getInstance().debug("Simulating right click at (" + std::to_string(point.x) + ", " + std::to_string(point.y) + ")");
    SetCursorPos(point.x, point.y); // Move cursor to the point
    mouse_event(MOUSEEVENTF_RIGHTDOWN, point.x, point.y, 0, 0);
    mouse_event(MOUSEEVENTF_RIGHTUP, point.x, point.y, 0, 0);
}

void ClickScript::simulateEnterKey(const char &key)
{
    // Implementation for simulating pressing the enter key
    MyLogger::getInstance().debug("Simulating enter key press");
    if (key == '\n' || key == '\r')
    {
        // Simulate pressing the enter key
        keybd_event(VK_RETURN, 0, 0, 0);               // Key down
        keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0); // Key up
    }
    else
    {
        MyLogger::getInstance().error("Invalid key for enter simulation: " + std::string(1, key));
    }
}

void ClickScript::simulateDelay(int delay)
{
    // TODO : time transformation
    Sleep(300); // Convert milliseconds to seconds for sleep
}

void ClickScript::addBehavior(const Behavior &behavior)
{
    behaviors.push_back(behavior);
    MyLogger::getInstance().debug("Behavior added: " + std::to_string(behavior.action));
}

ClickScript::ClickScript()
{
    MyLogger::getInstance().info("ClickScript initialized.");
}

void ClickScript::load_ClickScript_fromfile(const std::string &filename)
{
    MyLogger::getInstance().info("Loading ClickScript from file: " + filename);
    std::ifstream file(filename);

    if (!file.is_open())
    {
        MyLogger::getInstance().error("Failed to open ClickScript file: " + filename);
        return;
    }

    std::string line;
    bool inCommandBlock = false;

    // Clear existing behaviors
    behaviors.clear();

    while (std::getline(file, line))
    {
        // Remove leading and trailing whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Skip empty lines
        if (line.empty())
            continue;

        // Check for markers
        if (line == "#start")
        {
            inCommandBlock = true;
            MyLogger::getInstance().debug("Found start marker");
            continue;
        }
        else if (line == "#end")
        {
            inCommandBlock = false;
            MyLogger::getInstance().debug("Found end marker");
            break;
        }

        // Only process data inside the command block
        if (inCommandBlock)
        {
            Behavior behavior = parseCommandLine(line);
            if (behavior.action != NONE)
            {
                addBehavior(behavior);
                MyLogger::getInstance().debug("Parsed command: " + line);
            }
            else
            {
                MyLogger::getInstance().warning("Invalid or ignored command: " + line);
            }
        }
    }

    file.close();
    MyLogger::getInstance().info("Loaded " + std::to_string(behaviors.size()) + " behaviors");
}

void ClickScript::save_ClickScript_tofile(const std::string &filename)
{
    // TODO: Implementation for saving ClickScript to a file
    MyLogger::getInstance().info("Saving ClickScript to file: " + filename);
}

void ClickScript::print_ClickScript()
{
    if (behaviors.empty())
    {
        MyLogger::getInstance().warning("No behaviors to print in ClickScript.");
        return;
    }
    system("cls");
    std::cout << "--- ClickScript Checklist ---" << std::endl;
    std::cout << "Loops: " << loops << std::endl;
    for (const auto &behavior : behaviors)
    {
        switch (behavior.action)
        {
        case LEFT_CLICK:
            std::cout << "LEFT: " << behavior.point.x << " " << behavior.point.y << std::endl;
            break;
        case RIGHT_CLICK:
            std::cout << "RIGHT: " << behavior.point.x << " " << behavior.point.y << std::endl;
            break;
        case DELAY:
            std::cout << "DELAY: " << behavior.delay << std::endl;
            break;
        case ENTER_KEY:
            std::cout << "Press Enter" << std::endl;
            break;
        default:
            break;
        }
    }
    std::cout << "-----------------------------" << std::endl;

    std::cout << "Press any key to continue..." << std::endl;
    std::cin.get();
}

Behavior ClickScript::parseCommandLine(const std::string &line)
{
    Behavior behavior;
    behavior.action = NONE;
    behavior.point = {0, 0};
    behavior.key = '\0';
    behavior.delay = 0;

    std::istringstream iss(line);
    std::string command;

    // Read the first word as the command
    if (!(iss >> command))
    {
        MyLogger::getInstance().error("Failed to parse command line: " + line);
        return behavior; // Empty or invalid line
    }

    if (command == "LEFT")
    {
        int x, y;
        if (iss >> x >> y)
        {
            behavior.action = LEFT_CLICK;
            behavior.point.x = x;
            behavior.point.y = y;
            MyLogger::getInstance().debug("Parsed LEFT click at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
        }
        else
        {
            MyLogger::getInstance().error("LEFT command requires two coordinates");
        }
    }
    else if (command == "RIGHT")
    {
        int x, y;
        if (iss >> x >> y)
        {
            behavior.action = RIGHT_CLICK;
            behavior.point.x = x;
            behavior.point.y = y;
            MyLogger::getInstance().debug("Parsed RIGHT click at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
        }
        else
        {
            MyLogger::getInstance().error("RIGHT command requires two coordinates");
        }
    }
    else if (command == "DELAY")
    {
        int delayMs;
        if (iss >> delayMs)
        {
            behavior.action = DELAY;
            behavior.delay = delayMs;
            MyLogger::getInstance().debug("Parsed DELAY of " + std::to_string(delayMs) + "ms");
        }
        else
        {
            MyLogger::getInstance().error("DELAY command requires a duration");
        }
    }
    else if (command == "ENTER")
    {
        behavior.action = ENTER_KEY;
        behavior.key = '\n'; // or '\r'
        MyLogger::getInstance().debug("Parsed ENTER key press");
    }
    else
    {
        // Unknown command, enhance error robustness
        MyLogger::getInstance().warning("Unknown command: " + command + " - line ignored");
        behavior.action = NONE;
    }

    return behavior;
}

int ClickScript::get_loops()
{
    std::cout << "Please enter the number of loops: ";
    std::cin >> loops;
    MyLogger::getInstance().debug("Retrieving number of loops: " + std::to_string(loops));
    return loops;
}
