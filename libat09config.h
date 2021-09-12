#ifndef LIBAT09_CONFIG_H
#define LIBAT09_CONFIG_H

#define DEBUG

// Uncomment to disable superfluous API functions. Example: AT09::SetName() etc.
//   All commands will need to be sent manually via AT09::setCommand()
//#define MINIMUM

#ifndef MINIMUM
    // Uncomment to enable "get" functions that read from the chip. Example: AT09::GetFWVersion() etc.
    #define GETTERS

    // Uncomment to enable "set" functions that write settings to the chip. Example: AT09::SetName() etc.
    #define SETTERS
#endif

#endif //LIBAT09_CONFIG_H
