// TODO: Description support for ClickScript
// TODO: Log System
// TODO: what if load task failed
// TODO: make the countdown changeable
// TODO: *clk* path support

#define MAIN_RELEASE 1

#include <iostream>
#include <iostream>
#include "MyLogger.h"
#include "Config.h"
#include "system.h"
#include "clickscript.h"

#if MAIN_RELEASE

#include <iostream>
#include "MyLogger.h"
#include "Config.h"
#include "system.h"
#include "clickscript.h"

int main()
{
    System system;
    system.initialize();

    system.runMainLoop(); // Run the main loop
    return 0;
}

#else

int main()
{
    Config config;
    config.load();
    config.print();
    std::cout << "Path1: " << config.get("PATH_1") << std::endl;
    return 0;
}
#endif