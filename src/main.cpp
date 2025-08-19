// TODO: Description support for ClickScript
// TODO: Log System
// TODO: what if load task failed
// TODO: make the countdown changeable
// TODO: *clk* path support

#define MAIN_RELEASE 1

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

#include <iostream>

int main()
{
}

#endif