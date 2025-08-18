#ifndef CLICKSCRIPT_H
#define CLICKSCRIPT_H

// C++ standard library headers
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// System-specific headers
#include <windows.h> // For Windows API functions like SetCursorPos and mouse_event

// Project local headers
#include "MyLogger.h"
#include "system.h"

#define ERR_DELAY 300 // delay in milliseconds for error messages

typedef enum
{
    LEFT_CLICK,
    RIGHT_CLICK,
    ENTER_KEY,
    DELAY,
    NONE
} Action;

typedef struct
{
    int x = -1;
    int y = -1;
} Point;

typedef struct
{
    Point point;   // Click position
    char key = -1; // Key to simulate click
    int delay = -1;

    Action action = NONE;
} Behavior;

class ClickScript
{
public:
    ClickScript();
    void addBehavior(const Behavior &behavior);
    void removeBehavior(int index);
    void execute();
    void assert_behavior();
    void save_ClickScript_tofile(const std::string &filename);
    void load_ClickScript_fromfile(const std::string &filename);
    void print_ClickScript();
    int get_loops();
    Behavior parseCommandLine(const std::string &line);

    // Simulate click && press functions
    void simulateLeftClick(const Point &point);
    void simulateRightClick(const Point &point);
    void simulateEnterKey(const char &key);
    void simulateDelay(int delay);

private:
    std::string filename;
    std::string description;
    std::vector<Behavior> behaviors;
    int loops = 0;
};
#endif // CLICKSCRIPT_H