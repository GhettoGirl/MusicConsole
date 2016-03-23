#ifndef KBHIT_HPP
#define KBHIT_HPP

#include <qsystemdetection.h>
#include <iostream>

// Waits a specific time for user response to break out of a(n) (infinite) loop.
// Break key == Carriage Return [ONLY]

// works on most UNIX systems and Windows

// the default inveral for MusicConsole is a half second


//// HOW TO USE
//
// use the KBHIT macro to start the breakable infinite loop
// and use the KBHIT_END macro to end the infinite loop
//
// NOTE: the input "\r" is sent, we need to catch this input and clear it
//       do prevent undefined or unexpected behavior
//
// use the KBHIT_NOT_INFINITE(loop end condition) macro instead, if your loop should not be infinite
// and use KBHIT_NOT_INFINITE_END after your code to close the non-infinite loop
//
// DON'T APPEND A SEMI-COLON (;) TO THE MACROS !!

#ifdef Q_OS_WIN32
#include <conio.h>
void usleep(__int64 usec);
#define KBHIT while (true) { usleep(500000); if (kbhit()) break;
#else // UNIX
#define KBHIT while (!kbhit(0, 500000)) {
#endif

// start of non-infinite loop
#define KBHIT_NOT_INFINITE(KBHIT_LOOP_END_CONDITION) \
    bool kbhit_normal_loop_end = false; \
    KBHIT \
        if (KBHIT_LOOP_END_CONDITION) { \
            kbhit_normal_loop_end = true; \
            break; \
        }

// end of non-infinite loop
#define KBHIT_NOT_INFINITE_END } \
    if (!kbhit_normal_loop_end) { \
        std::cin.clear(); \
        std::cin.get(); \
    }

// end of kbhit infinite loop
#define KBHIT_END \
    } \
    std::cin.clear(); \
    std::cin.get();

#ifndef Q_OS_WIN32
int kbhit(int seconds = 0, int microseconds = 0);
#endif

#endif // KBHIT_HPP
