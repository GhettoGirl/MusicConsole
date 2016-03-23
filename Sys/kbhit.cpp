#include "kbhit.hpp"

#ifndef Q_OS_WIN32
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/select.h>

int kbhit(int seconds, int microseconds)
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = seconds;
    tv.tv_usec = microseconds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}
#else // Windows
#include <windows.h>
void usleep(__int64 usec)
{
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}
#endif
